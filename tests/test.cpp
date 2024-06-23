#include "safe-map.hpp"

using namespace Safe;
struct Entity {
    Entity(const string& name)
        : name(name) {}
    string name;
    List<string> components;
};
int main() {
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
    auto view = ReadView(2, map);
    cout << view->name << endl;
    map.forEach([](int key, ReadLocked<Entity>& entity) {
        cout << key << ": " << entity->name << endl;
    });
    return EXIT_SUCCESS;
}