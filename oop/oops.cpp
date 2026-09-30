// Encapsulation, abstraction, inheritance and polymorphism

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// Abstraction: every vehicle can refuel, drive and report its cost; how much
// fuel it burns is left to the derived classes
class Vehicle {
private:
  // Encapsulation: fuel only changes through refuel() and drive()
  double tankSize;
  double fuel = 0;

protected:
  string name;

public:
  Vehicle(string name, double tankSize) : tankSize(tankSize), name(name) {}
  virtual ~Vehicle() = default;

  double getFuel() const { return fuel; }

  // Adds fuel up to the tank size; returns the litres actually added
  double refuel(double litres) {
    if (litres <= 0)
      return 0;
    double added = min(litres, tankSize - fuel);
    fuel += added;
    return added;
  }

  // Drives km kilometres if there is enough fuel
  bool drive(double km) {
    double needed = km * litresPerKm();
    if (needed > fuel)
      return false;
    fuel -= needed;
    return true;
  }

  virtual double litresPerKm() const = 0;

  virtual double costPerKm(double fuelPrice) const {
    return litresPerKm() * fuelPrice;
  }

  virtual void print(double fuelPrice) const {
    cout << name << ": " << costPerKm(fuelPrice) << " per km, " << fuel
         << " L left" << endl;
  }
};

// Inheritance: each vehicle reuses Vehicle's fuel handling
class Car : public Vehicle {
public:
  Car(string name) : Vehicle(name, 45) {}
  double litresPerKm() const override { return 0.07; }
};

class SportsCar : public Car {
public:
  SportsCar(string name) : Car(name) {}
  double litresPerKm() const override { return 0.12; }
};

class Bike : public Vehicle {
public:
  Bike(string name) : Vehicle(name, 12) {}
  double litresPerKm() const override { return 0.025; }
};

class Truck : public Vehicle {
private:
  double loadTonnes;

public:
  Truck(string name, double loadTonnes)
      : Vehicle(name, 300), loadTonnes(loadTonnes) {}

  // A heavier load burns more fuel
  double litresPerKm() const override { return 0.25 + 0.02 * loadTonnes; }

  // Tolls on top of fuel
  double costPerKm(double fuelPrice) const override {
    return Vehicle::costPerKm(fuelPrice) + 1.5;
  }

  void print(double fuelPrice) const override {
    Vehicle::print(fuelPrice);
    cout << "  " << name << " carries " << loadTonnes << " t" << endl;
  }
};

int main() {
  vector<unique_ptr<Vehicle>> fleet;
  fleet.push_back(make_unique<Car>("Car"));
  fleet.push_back(make_unique<Bike>("Bike"));
  fleet.push_back(make_unique<Truck>("Truck", 10));

  // Polymorphism: one loop, each vehicle uses its own litresPerKm / print
  const double fuelPrice = 100; // per litre
  for (auto &v : fleet) {
    v->refuel(1000); // capped at the tank size
    v->drive(100);
    v->print(fuelPrice);
  }

  // fleet[0]->fuel = 500; // doesn't compile: fuel is private

  // Pointer and reference use the real object; a plain copy is sliced to Car
  SportsCar sports("Sports car");
  Car *p = &sports;
  Car &r = sports;
  Car c = sports;
  cout << "pointer:   " << p->litresPerKm() << " L/km" << endl; // 0.12
  cout << "reference: " << r.litresPerKm() << " L/km" << endl;  // 0.12
  cout << "copy:      " << c.litresPerKm() << " L/km" << endl;  // 0.07
  return 0;
}
