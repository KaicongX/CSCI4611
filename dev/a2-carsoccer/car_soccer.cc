/** CSci-4611 Assignment 2:  Car Soccer
*/

#define _USE_MATH_DEFINES
#include "car_soccer.h"
#include "config.h"
//#define M_PI 3.14159


CarSoccer::CarSoccer() : GraphicsApp(1024, 768, "Car Soccer") {
	// Define a search path for finding data files (images and shaders)
	searchPath_.push_back(".");
	searchPath_.push_back("./data");
	searchPath_.push_back(DATA_DIR_INSTALL);
	searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer() {
}


Vector2 CarSoccer::joystick_direction() {
	Vector2 dir;
	if (IsKeyDown(GLFW_KEY_LEFT))
		dir[0]--;
	if (IsKeyDown(GLFW_KEY_RIGHT))
		dir[0]++;
	if (IsKeyDown(GLFW_KEY_UP))
		dir[1]++;
	if (IsKeyDown(GLFW_KEY_DOWN))
		dir[1]--;
	return dir;
}


void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
	if (key == GLFW_KEY_SPACE) {
		double sd = (rand() % 101) / 100.0 * M_PI;
		ball_.Reset();
		ball_.set_velocity(Vector3(25 * cos(sd), 15, 25 * sin(sd)));
	}
	if (key == GLFW_KEY_F1) {
		ball_.Reset();
		car_.Reset();
	}
}


void CarSoccer::UpdateSimulation(double timeStep) {
	//Check if ball is in goal
	if ((ball_.position()[0] >= (-10 + ball_.radius())) && ((ball_.position()[0] <= (10 - ball_.radius()))) &&
		(ball_.position()[1] <= (10 - ball_.radius())) &&
		((ball_.position()[2] <= (-50 + ball_.radius())) || (ball_.position()[2] >= (50 - ball_.radius())))) {
		ball_.Reset();
		car_.Reset();
	}

	//Car movement

	Vector2 dir = joystick_direction();
	if (dir[1] == 0) {
		//If keys are not being pressed, slow the car down
		Vector3 drag = car_.drag()*car_.velocity();
		car_.set_velocity(car_.velocity() - drag*timeStep);
		car_.set_position(car_.position() + car_.velocity()*timeStep);

		//Check for wall collisions
		if (car_.position()[0] > (40 - car_.collision_radius())) {
			car_.set_position(Point3(40 - car_.collision_radius(), car_.position()[1], car_.position()[2]));
			car_.set_velocity(Vector3(0, 0, 0));
		}
		if (car_.position()[0] < (-40 + car_.collision_radius())) {
			car_.set_position(Point3(-40 + car_.collision_radius(), car_.position()[1], car_.position()[2]));
			car_.set_velocity(Vector3(0, 0, 0));
		}
		if (car_.position()[2] > (50 - car_.collision_radius())) {
			car_.set_position(Point3(car_.position()[0], car_.position()[1], 50 - car_.collision_radius()));
			car_.set_velocity(Vector3(0, 0, 0));
		}
		if (car_.position()[2] < (-50 + car_.collision_radius())) {
			car_.set_position(Point3(car_.position()[0], car_.position()[1], -50 + car_.collision_radius()));
			car_.set_velocity(Vector3(0, 0, 0));
		}
	}
	else {
		//If keys are being pressed
		if (dir.Length() > 1) {
			dir = Vector2::Normalize(dir);
		}
		//Rotate and move car
		car_.set_rotation(car_.rotation() + (-dir[0] * car_.rotation_delta()*timeStep));
		Vector3 thrust = car_.thrust()*Vector3(-dir[1] * sin(car_.rotation()*M_PI / 180), 0, -dir[1] * cos(car_.rotation()*M_PI / 180));
		Vector3 drag = car_.drag()*car_.velocity();
		car_.set_velocity(car_.velocity() + (thrust - drag)*timeStep);
		car_.set_position(car_.position() + car_.velocity()*timeStep);

		//Check for wall collisions
		if (car_.position()[0] > (40 - car_.collision_radius())) {
			car_.set_position(Point3(40 - car_.collision_radius(), car_.position()[1], car_.position()[2]));
			car_.set_velocity(Vector3(0, 0, 0));
			car_.set_rotation(car_.rotation() - (-dir[0] * car_.rotation_delta()*timeStep));
		}
		if (car_.position()[0] < (-40 + car_.collision_radius())) {
			car_.set_position(Point3(-40 + car_.collision_radius(), car_.position()[1], car_.position()[2]));
			car_.set_velocity(Vector3(0, 0, 0));
			car_.set_rotation(car_.rotation() - (-dir[0] * car_.rotation_delta()*timeStep));
		}
		if (car_.position()[2] > (50 - car_.collision_radius())) {
			car_.set_position(Point3(car_.position()[0], car_.position()[1], 50 - car_.collision_radius()));
			car_.set_velocity(Vector3(0, 0, 0));
			car_.set_rotation(car_.rotation() - (-dir[0] * car_.rotation_delta()*timeStep));
		}
		if (car_.position()[2] < (-50 + car_.collision_radius())) {
			car_.set_position(Point3(car_.position()[0], car_.position()[1], -50 + car_.collision_radius()));
			car_.set_velocity(Vector3(0, 0, 0));
			car_.set_rotation(car_.rotation() - (-dir[0] * car_.rotation_delta()*timeStep));
		}
	}

	//Bounce ball off walls and ceilings
	if (ball_.position()[1] < (ball_.radius())) {
		ball_.set_velocity(Vector3(ball_.velocity()[0], fabs(ball_.velocity()[1]), ball_.velocity()[2])*0.8);
		ball_.set_position(Point3(ball_.position()[0], ball_.radius(), ball_.position()[2]));
	}
	if (ball_.position()[1] > (35 - ball_.radius())) {
		ball_.set_velocity(Vector3(ball_.velocity()[0], -fabs(ball_.velocity()[1]), ball_.velocity()[2])*0.8);
		ball_.set_position(Point3(ball_.position()[0], 35 - ball_.radius(), ball_.position()[2]));
	}
	if (ball_.position()[0] < (-40 + ball_.radius())) {
		ball_.set_velocity(Vector3(fabs(ball_.velocity()[0]), ball_.velocity()[1], ball_.velocity()[2])*0.8);
		ball_.set_position(Point3(-40 + ball_.radius(), ball_.position()[1], ball_.position()[2]));
	}
	if (ball_.position()[0] > (40 - ball_.radius())) {
		ball_.set_velocity(Vector3(-fabs(ball_.velocity()[0]), ball_.velocity()[1], ball_.velocity()[2])*0.8);
		ball_.set_position(Point3(40 - ball_.radius(), ball_.position()[1], ball_.position()[2]));
	}
	if (ball_.position()[2] < (-50 + ball_.radius())) {
		ball_.set_velocity(Vector3(ball_.velocity()[0], ball_.velocity()[1], fabs(ball_.velocity()[2]))*0.8);
		ball_.set_position(Point3(ball_.position()[0], ball_.position()[1], -50 + ball_.radius()));
	}
	if (ball_.position()[2] > (50 - ball_.radius())) {
		ball_.set_velocity(Vector3(ball_.velocity()[0], ball_.velocity()[1], -fabs(ball_.velocity()[2]))*0.8);
		ball_.set_position(Point3(ball_.position()[0], ball_.position()[1], 50 - ball_.radius()));
	}


	ball_.set_velocity(ball_.velocity() + ball_.gravity()*timeStep);

	//Handle collision between ball and car
	Vector3 ball2car = ball_.position() - car_.position();
	if (ball2car.Length() < (ball_.radius() + car_.collision_radius())) {
		ball_.set_position(car_.position() + (ball_.radius() + car_.collision_radius())*Vector3::Normalize(ball2car));
		Vector3 r_vel = ball_.velocity() - car_.velocity();
		ball_.set_velocity(car_.velocity() + (r_vel - 2 * (r_vel.Dot(Vector3::Normalize(ball2car))*Vector3::Normalize(ball2car))));
	}

	//Update ball position
	ball_.set_position(ball_.position() + ball_.velocity()*timeStep);

}


void CarSoccer::InitOpenGL() {
	// Set up the camera in a good position to see the entire field
	projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
	modelMatrix_ = Matrix4::LookAt(Point3(0, 60, 70), Point3(0, 0, 10), Vector3(0, 1, 0));

	// Set a background color for the screen
	glClearColor(0.8, 0.8, 0.8, 1);

	// Load some image files we'll use
	fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
	crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}


void CarSoccer::DrawUsingOpenGL() {
	// Draw the crowd as a fullscreen background image
	quickShapes_.DrawFullscreenTexture(Color(1, 1, 1), crowdTex_);

	// Draw the field with the field texture on it.
	Color col(16.0 / 255.0, 46.0 / 255.0, 9.0 / 255.0);
	Matrix4 M = Matrix4::Translation(Vector3(0, -0.201, 0)) * Matrix4::Scale(Vector3(50, 1, 60));
	quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
	M = Matrix4::Translation(Vector3(0, -0.2, 0)) * Matrix4::Scale(Vector3(40, 1, 50));
	quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1, 1, 1), fieldTex_);

	// Draw the car
	Color carcol(0.8, 0.2, 0.2);
	Matrix4 Mcar =
		Matrix4::Translation(car_.position() - Point3(0, 0, 0)) *
		Matrix4::Scale(car_.size()) *
		Matrix4::Scale(Vector3(0.5, 0.5, 0.5)) *
		Matrix4::Rotation(Point3(0, 0, 0), Vector3(0, 1, 0), car_.rotation()*M_PI / 180);
	quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);


	// Draw the ball
	Color ballcol(1, 1, 1);
	Matrix4 Mball =
		Matrix4::Translation(ball_.position() - Point3(0, 0, 0)) *
		Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
	quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);


	// Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
	// flattens the sphere into a pancake, which we then draw just a bit
	// above the ground plane.
	Color shadowcol(0.2, 0.4, 0.15);
	Matrix4 Mshadow =
		Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
		Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
		Matrix4::RotationX(90);
	quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);


	// You should add drawing the goals and the boundary of the playing area
	Matrix4 Mboarderbot;
	std::vector<Point3> boarderbot_points;
	boarderbot_points.push_back(Point3(40, 0, 50));
	boarderbot_points.push_back(Point3(40, 0, -50));
	boarderbot_points.push_back(Point3(-40, 0, -50));
	boarderbot_points.push_back(Point3(-40, 0, 50));
	quickShapes_.DrawLines(modelMatrix_ * Mboarderbot, viewMatrix_, projMatrix_, Color(1, 1, 1), boarderbot_points, QuickShapes::LinesType::LINE_LOOP, 0.25);

	Matrix4 Mboardertop;
	std::vector<Point3> boardertop_points;
	boardertop_points.push_back(Point3(-40, 35, 50));
	boardertop_points.push_back(Point3(-40, 35, -50));
	boardertop_points.push_back(Point3(40, 35, -50));
	boardertop_points.push_back(Point3(40, 35, 50));
	quickShapes_.DrawLines(modelMatrix_ * Mboardertop, viewMatrix_, projMatrix_, Color(1, 1, 1), boardertop_points, QuickShapes::LinesType::LINE_LOOP, 0.25);

	Matrix4 Mboardersides;
	std::vector<Point3> boardersides_points;
	boardersides_points.push_back(Point3(40, 0, 50));
	boardersides_points.push_back(Point3(40, 35, 50));
	boardersides_points.push_back(Point3(-40, 0, 50));
	boardersides_points.push_back(Point3(-40, 35, 50));
	boardersides_points.push_back(Point3(-40, 0, -50));
	boardersides_points.push_back(Point3(-40, 35, -50));
	boardersides_points.push_back(Point3(40, 0, -50));
	boardersides_points.push_back(Point3(40, 35, -50));
	quickShapes_.DrawLines(modelMatrix_ * Mboardersides, viewMatrix_, projMatrix_, Color(1, 1, 1), boardersides_points, QuickShapes::LinesType::LINES, 0.25);
	Matrix4 Mgoal1;
	std::vector<Point3> goal1_points;
	goal1_points.push_back(Point3(10, 0, 50));
	goal1_points.push_back(Point3(-10, 0, 50));
	goal1_points.push_back(Point3(-10, 10, 50));
	goal1_points.push_back(Point3(10, 10, 50));
	quickShapes_.DrawLines(modelMatrix_ * Mgoal1, viewMatrix_, projMatrix_, Color(1, 0, 0), goal1_points, QuickShapes::LinesType::LINE_LOOP, 0.5);

	Matrix4 Mgoal2;
	std::vector<Point3> goal2_points;
	goal2_points.push_back(Point3(10, 0, -50));
	goal2_points.push_back(Point3(-10, 0, -50));
	goal2_points.push_back(Point3(-10, 10, -50));
	goal2_points.push_back(Point3(10, 10, -50));
	quickShapes_.DrawLines(modelMatrix_ * Mgoal2, viewMatrix_, projMatrix_, Color(0, 1, 0), goal2_points, QuickShapes::LinesType::LINE_LOOP, 0.5);
}
