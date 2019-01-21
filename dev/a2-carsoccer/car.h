/** CSci-4611 Assignment 2:  Car Soccer
*/

#ifndef CAR_H_
#define CAR_H_

#include <mingfx.h>

/// Small data structure for a car
class Car {
public:

	/// The constructor sets the static properties of the car, like its size,
	/// and then calls Reset() to reset the position, velocity, and any other
	/// dynamic variables that change during game play.
	Car() : size_(3, 2, 4), collision_radius_(2.5) {
		Reset();
		velocity_ = Vector3(0, 0, 0);
		rotation_ = 0.0;
		thrust_ = 250;
		rotation_delta_ = 200;
		drag_ = 5;
	}

	/// Nothing special needed in the constructor
	virtual ~Car() {}

	/// Resets all the dynamic variables, so if you call this after a goal, the
	/// car will go back to its starting position.
	void Reset() {
		position_ = Point3(0, size_[1] / 2, 45);
		velocity_ = Vector3(0, 0, 0);
		rotation_ = 0.0;
	}

	float collision_radius() { return collision_radius_; }

	Vector3 size() { return size_; }

	Point3 position() { return position_; }
	void set_position(const Point3 &p) { position_ = p; }

	Vector3 velocity() { return velocity_; }
	void set_velocity(Vector3 &v) { velocity_ = v; }

	float rotation() { return rotation_; }
	void set_rotation(float r) { rotation_ = r; }

	float thrust() { return thrust_; }
	float drag() { return drag_; }
	float rotation_delta() { return rotation_delta_; }
private:
	// You will probably need to store some additional data here, e.g., speed.

	Vector3 size_;
	float collision_radius_;
	Point3 position_;
	Vector3 velocity_;
	float rotation_;
	float rotation_delta_;
	float thrust_;
	float drag_;
};

#endif
