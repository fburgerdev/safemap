#include "safemap.hpp"

using namespace Memory;
struct Entity {
    Entity(const string& name)
        : name(name) {}
    string name;
    List<string> components;
};
int main() {
    Collection<int> typemap;
    typemap.addType<Entity>();
    typemap.emplace<Entity>(1, "ENTT");
    cout << typemap.readLock<Entity>(1)->name << endl;

    SecureMap<int, Entity> map;
    map.emplace(1, "Entity1");
    map.emplace(2, "Entity2");
    auto entity = map.writeLock(2);
    if (entity) {
        entity->name = "HALLO";
        cout << entity->name << endl;
    }
    entity.release();
    map.destroy(1);
    map.clean();
    ReadView<int, Entity> view = WriteView(2, map);
    cout << view->name << endl;
    map.forEach([](int key, ReadLocked<Entity>& entity) {
        cout << key << ": " << entity->name << endl;
    });

    Set<GenericView<int>> views;
    views.emplace(GenericView<int>(2, map));
    for (auto& view : views)
        cout << ReadView<int, Entity>(view)->name << endl;
    return EXIT_SUCCESS;
}