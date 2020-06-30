#include <stdio.h>

#include "Entity\EntityManager.hpp"
#include "Entity\Entity.hpp"

struct Position
{
	Position() : x(0), y(0) {}
	Position(int x, int y) : x(x), y(y) {}
	int x, y;
};

void creation(cs::EntityManager& m)
{
	auto c1 = 10;
	auto c2 = 240.f;

	auto e1 = m.create(); // Empty
	auto e2 = m.create(c1, c2); // Use component
	auto e3 = m.create<int>(22); // Construct component

	assert(e2.has<int>() && e2.component<int>() == 10);
	assert(e3.has<int>() && e3.component<int>() == 22);
	assert(e2.has<float>() && e2.component<float>() == 240.f);
}

void assignment(cs::EntityManager& m)
{
	auto c1 = 10;
	auto c2 = 240.f;

	auto e1 = m.create();
	auto e2 = m.create();

	e1.assign<int>(40); // Construct component
	e1.assign(c2); // Use component
	e2.assign(c1, c2); // Use components
	e2.assign<Position>(22, 33); // Construct component

	assert(e1.has<int>() && e1.component<int>() == 40);
	assert(e1.has<float>() && e1.component<float>() == 240.f);
	assert(e2.has<int>() && e2.component<int>() == 10);
	assert(e2.has<float>() && e2.component<float>() == 240.f);
	assert(e2.has<Position>() && e2.component<Position>().x == 22);
}

void replacement(cs::EntityManager& m)
{
	auto c1 = 10;
	auto c2 = 240.f;
	auto c3 = 30;
	auto c4 = 320.f;

	auto e1 = m.create(c1, c2);
	auto e2 = m.create<int>(34);

	e1.replace(c3, c4); // Use components
	e1.replace<int>(60); // Construct component
	e2.replace(c1); // Use component
	//e2.replace<float>(30.bar); // Theres no float component: error!

	assert(e1.has<int>() && e1.component<int>() == 60);
	assert(e1.has<float>() && e1.component<float>() == 320.f);
	assert(e2.has<int>() && e2.component<int>() == 10);
}

void accomodation(cs::EntityManager& m)
{
	auto c1 = 30;
	auto c2 = 320.f;

	auto e1 = m.create(c1, c2);
	auto e2 = m.create<int>(34);

	e1.accomodate<int>(2); // Build and replace
	e1.accomodate<float>(50.f); // Build and replace
	e2.accomodate(c1); // Replace
	e2.accomodate(c2); // Assign
	e2.accomodate<double>(68.0); // Build and assign

	assert(e1.has<int>() && e1.component<int>() == 2);
	assert(e1.has<float>() && e1.component<float>() == 50.f);
	assert(e2.has<float>() && e2.component<float>() == 320.f);
	assert(e2.has<double>() && e2.component<double>() == 68.0);
}

void resetion(cs::EntityManager& m)
{
	auto c1 = 89;
	auto c2 = 76.f;
	auto c3 = 12;

	auto e1 = m.create(c1);
	auto e2 = m.create(c1, c2);

	e1.reset(c3);
	e1.reset<double>(); // Do nothing
	e2.reset<int, float>();

	assert(!e2.has<double>());
	assert(!e1.has<int>());
	assert(!e2.has<int>());
	assert(!e2.has<float>());
}

void remotion(cs::EntityManager& m)
{
	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m.create(c1, c2);
	auto e2 = m.create(c2);

	e1.remove<int, float>();
	e2.remove(c2);

	assert(!e1.has<int>());
	assert(!e1.has<float>());
	assert(!e2.has<float>());
}

void checkup(cs::EntityManager& m) {
	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m.create(c1, c2);
	auto e2 = m.create(c2);

	assert(e1.has<int>());
	assert(e2.has<float>());
	assert(e2.has<float>());
	assert(!e1.has<double>());
}

void counting() {
	cs::EntityManager m;

	auto c1 = 4;
	auto c2 = 30.f;

	auto e1 = m.create<int>();
	auto e2 = m.create(c1, c2);

	assert(m.count<int>() == 2);
	assert(m.count<float>() == 1);
}

void sizes() {
	cs::EntityManager m;

	auto e1 = m.create();
	auto e2 = m.create();

	assert(m.size() == 2U);
}

void capacities() {
	cs::EntityManager m;

	auto e1 = m.create();
	auto e2 = m.create();
	auto e3 = m.create();

	e1.destroy();
	e2.destroy();

	assert(m.size() == 1U);
	assert(m.capacity() == 3U);
}

void emptyness()
{
	cs::EntityManager m;

	auto e1 = m.create<int>();
	auto e2 = m.create<int>();
	auto e3 = m.create<float>();

	// Component emptyness
	auto c1 = m.empty<int>();
	auto c2 = m.empty<int, float>();
	auto c3 = m.empty<int, double>();

	// Entity emptyness
	auto c4 = m.empty();

	assert(!c1);
	assert(!c2);
	assert(c3);
	assert(!c4);
}

void components(cs::EntityManager& m) {
	auto e1 = m.create<int>(1);
	auto e2 = m.create<float>(2.f);

	e2.assign<double>(3.0);

	auto c0 = e1.components<>();
	auto c1 = e1.components<int>();
	auto c2 = e2.components<float, double>();
}

void iteration(cs::EntityManager& m) {
	m.each([](cs::Entity e) {
		printf("Iterating entity %d (v%d)...\n", e.id(), e.version());
	});
}

int main()
{
	cs::EntityManager m;

	creation(m);
	assignment(m);
	replacement(m);
	accomodation(m);
	resetion(m);
	remotion(m);
	checkup(m);
	counting();
	sizes();
	capacities();
	emptyness();
	components(m);
	//iteration(m);

	auto c1 = m.count<int>();
	auto c2 = m.count<float>();
	auto c3 = m.count<double>();
	auto c4 = m.count<Position>();

	m.each<int>([](auto e, int& i) {
		printf("Entity %d has int component: %d \n", e, i);
	});
	m.each<float>([](auto e, float& i) {
		printf("Entity %d has float component: %.2f \n", e, i);
	});
	m.each<int, float>([](auto e, int& i, float& f) {
		printf("Entity %d has both int and float components: %d (int) and %.2f (float) \n", e, i, f);
	});

	return getchar();
}