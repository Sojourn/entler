
template<typename Schema>
Entity<Schema>::Entity(EntityDatabase<Schema>& database, size_t entity_index)
    : database_(database)
    , entity_index_(entity_index)
{
}

template<typename Schema>
EntityId Entity<Schema>::get_id() const {
    return database_.entity_table_[entity_index_].entity_id;
}

template<typename Schema>
template<typename Schema::ComponentType component_type>
bool Entity<Schema>::has_component() const {
    static constexpr auto component_type_index = Schema::find_component_type(component_type);
    static_assert(component_type_index, "Unknown component type");

    const Database::EntityRecord& record = database_.get_entity_record(entity_index_);
    return record.component_mask.test(*component_type_index);
}

template<typename Schema>
bool Entity<Schema>::has_component(ComponentType component_type) const {
    static constexpr auto component_type_index = Schema::find_component_type(component_type);
    static_assert(component_type_index, "Unknown component type");

    const Database::EntityRecord& record = database_.get_entity_record(entity_index_);
    return record.component_mask.test(*component_type_index);
}

template<typename Schema>
bool Entity<Schema>::has_components(typename Schema::ComponentMask component_mask) const {
    const Database::EntityRecord& record = database_.get_entity_record(entity_index_);
    return (record.component_mask & component_mask) == component_mask;
}

template<typename Schema>
template<typename Schema::ComponentType component_type>
auto Entity<Schema>::get_component() -> Component<component_type>& {
    static constexpr auto component_type_index = Schema::find_component_type(component_type);
    static_assert(component_type_index, "Unknown component type");

    const Database::EntityRecord& record = database_.get_entity_record(entity_index_);
    assert(has_component<component_type>());

    size_t component_index = record.component_indexes[*component_type_index];
    return std::get<*component_type_index>(database_.component_tables_)[component_index];
}

template<typename Schema>
template<typename Schema::ComponentType component_type>
auto Entity<Schema>::get_component() const -> const Component<component_type>& {
    static constexpr auto component_type_index = Schema::find_component_type(component_type);
    static_assert(component_type_index, "Unknown component type");

    const Database::EntityRecord& record = database_.get_entity_record(entity_index_);
    assert(has_component<component_type>());

    size_t component_index = record.component_indexes[*component_type_index];
    return std::get<*component_type_index>(database_.component_tables_)[component_index];
}

template<typename Schema>
EntityHandle<Schema>::EntityHandle(Entity<Schema>& entity)
        : entity_database_(&entity.database_)
        , entity_index_(entity.entity_index_)
{
    auto& record = entity_database_->get_entity_record(entity_index_);
    record.handles.push_back(*this);
}

template<typename Schema>
Entity<Schema> EntityHandle<Schema>::get() {
    assert(*this);
    return Entity<Schema>(*entity_database_, entity_index_);
}

template<typename Schema>
Entity<Schema> EntityHandle<Schema>::operator*() {
    return get();
}

template<typename Schema>
void EntityHandle<Schema>::reset() {
    if (*this) {
        entity_database_ = nullptr;
        entity_offset_ = 0;
        handle_list_node_.unlink();
    }
}

template<typename Schema>
void EntityHandle<Schema>::update_entity_index(size_t entity_index) {
    assert(*this);
    entity_index_ = entity_index;
}

template<typename Schema>
EntityObserver<Schema>::EntityObserver(EntityDatabase<Schema>& entity_database)
        : entity_database_(entity_database)
{
    entity_database_.add_entity_observer(*this);
}

template<typename Schema>
EntityObserver<Schema>::~EntityObserver() {
    entity_database_.remove_entity_observer(*this);
}

template<typename Schema>
EntityDatabase<Schema>::EntityDatabase()
        : next_entity_id_(0)
{
}

template<typename Schema>
template<typename Schema::ComponentType... component_types>
Entity<Schema> EntityDatabase<Schema>::add_entity(Component<component_types>... components) {
    EntityRecord record(next_entity_id_++);
    add_components(record, std::move(components)...);

    size_t entity_index = entity_table_.size();
    entity_table_.push_back(record);

    Entity<Schema> entity (*this, entity_index);
    notify_entity_added(entity);
    return entity;
}

template<typename Schema>
void EntityDatabase<Schema>::remove_entity(Entity<Schema> entity) {
    EntityRecord& record = entity_table_[entity.entity_index_];
    assert(record.entity_id >= 0);

    notify_entity_removed(entity);
    record.handles.clear();
    record.entity_id = -1;

    size_t component_type_index = 0;
    for_each_component_table([&](auto&& component_table) {
        if (record.component_mask.test(component_type_id)) {
            size_t component_index = record.component_indexes[component_type_index];

            // move the component onto the stack so it can free resources
            auto component = std::move(component_table[component_index]);
            (void)component;
        }

        component_type_index += 1;
    });
}

template<typename Schema>
template<typename Visitor>
void EntityDatabase<Schema>::for_each_entity(Visitor&& visitor) {
    size_t entity_count = entity_table_.size();
    for (size_t entity_index = 0; entity_index < entity_count; ++entity_index) {
        EntityRecord& record = entity_table_[entity_index];
        if (record.entity_id < 0) {
            continue;
        }

        visitor(Entity<Schema>(*this, entity_index));
    }
}

template<typename Schema>
template<typename Visitor>
void EntityDatabase<Schema>::for_each_entity(std::initializer_list<ComponentType> component_types, Visitor&& visitor) {
    ComponentMask component_mask;
    for (ComponentType component_type: component_types) {
        std::optional<size_t> component_type_index = Schema::find_component_type(component_type);
        assert(component_type_index);
        component_mask.set(*component_type_index);
    }

    size_t entity_count = entity_table_.size();
    for (size_t entity_index = 0; entity_index < entity_count; ++entity_index) {
        EntityRecord& record = entity_table_[entity_index];
        if (record.entity_id < 0) {
            continue;
        }
        if ((record.component_mask & component_mask) != component_mask) {
            continue;
        }

        visitor(Entity<Schema>(*this, entity_index));
    }
}
