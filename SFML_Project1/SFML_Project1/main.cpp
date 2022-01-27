/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 2021/10/28
Description:
This file is a practice for the using of SFML. I used sound, graphics, and keyboard events to create this
video game, which is similar to the Angry Birds. I added bounce to the Buzzy when it hits woodland creatures
and evil moscats. When a column disappeared, I made the image fell spinning down instead of just disappear.
I added sound effects. And I plot an auxiliary line to help player aiming.
*/

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <math.h>
#include <time.h>
#include <stdlib.h>

//game object
class buzzy_revenge {
private:
	int win_w = 1920;
	int win_h = 1080;

	//audio
	sf::SoundBuffer buffer1;
	sf::SoundBuffer buffer2;
	sf::Sound sound1;
	sf::Sound sound2;

	//game data
	int score = 0;
	int lives = 5;
	float buzzy_ang;
	float firing_ang;
	float power = 0; //  0 <= power <= 1
	float vi = 0; // vi = k * power + min
	float vx, vy;

	bool insect_appear = false;
	bool insect_ready = true;
	bool insect_lock = false;
	float insect_vx = -100;
	float insect_vy = 0;
	float insect_acc = 0; //acc vary from -max_acc / 2 to max_acc / 2
	const int insect_max_acc = 500;
	int vary_count = 0;
	sf::Vector2f insect_pos;
	sf::Vector2f insect_initpos;

	int hit_column = 0;
	int unicorn = -1;
	bool isbouncing = false;
	bool isfalling = false;
	bool ishit = false;
	const float k = 1000;
	const float min = 400;
	const float fall_speed = 500;
	const float spin = 720;
	const float gravity = 800;
	const float ang_vel = 60.0;
	const float power_acc = 0.8;
	const float initial_ang = -30.0;
	sf::Vector2f buzzy_initial;
	sf::Vector2f buzzy_pos;

	//interactive data
	bool buzz_launch = false;
	bool lock_key = false;
	bool key_pressed = false;
	sf::Keyboard::Key pressing_key;

	//graphics
	sf::RenderWindow window;

	sf::Event event;

	sf::Font font;
	sf::Text text;

	sf::Time elapsed;
	sf::Clock clock;

	sf::Time fall_time;
	sf::Clock fall_clock;

	sf::Time insect_time;
	sf::Clock insect_clock;

	std::vector<sf::Vector2f> all_pos;
	std::vector<sf::Sprite*> animal_pointer;
	int animal_pos[10];   //each position store an animal

	sf::Texture background_texture;
	sf::Sprite background;

	sf::Texture buzz_symbol_texture;
	sf::Sprite buzz_symbol;

	sf::Texture small_buzzy_texture;
	sf::Sprite small_buzzy;
	sf::Texture angry_unicorn_texture;
	sf::Sprite angry_unicorn;
	sf::Texture frog_texture;
	sf::Sprite frog;
	sf::Texture bulldog_texture;
	sf::Sprite bulldog;
	sf::Texture pig_texture;
	sf::Sprite pig;
	sf::Texture chicken_texture;
	sf::Sprite chicken;
	sf::Texture sheep_texture;
	sf::Sprite sheep;
	sf::Texture bunny_texture;
	sf::Sprite bunny;
	sf::Texture dog_texture;
	sf::Sprite dog;
	sf::Texture tiger_texture;
	sf::Sprite tiger;
	sf::Texture mouse_texture;
	sf::Sprite mouse;
	sf::Texture insect_texture;
	sf::Sprite insect;
	sf::Texture cloud_texture;
	sf::Sprite cloud;

	void draw_text(const std::string& text_string, int size, sf::Color color, sf::Vector2f position);
	void power_bar();
	void draw_lives();
	void update_buzzy();
	void key_action();
	void projectile();
	void auxiliary();
	void draw_animal();
	void random_animal();
	int crush(); //column can only be 1 or 2
	void hit_woodland_creature();
	void hit_mascot();
	void column_fall();
	void unicorn_fall();
	void check_level_up();
	void update_insect();

public:

	buzzy_revenge();
	void restart_game();
	void restart_buzzy();
	void restart_level();
	void draw_start_menu();
	void draw_game_gui();

};

//the class constructor
buzzy_revenge::buzzy_revenge() {
	elapsed = sf::milliseconds(0);
	buzzy_ang = initial_ang;
	firing_ang = initial_ang;
	vi = min;
	vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
	vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0);
	insect_initpos = sf::Vector2f(win_w / 16.0 * 17.0, win_h / 2);
	insect_pos = insect_initpos;
	srand(time(NULL));

	window.create(sf::VideoMode(win_w, win_h), "Buzzy's Revenge");

	
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 5; j++) {
			all_pos.push_back(sf::Vector2f(win_w / 16.0 * (15.0 - 2 * i), win_h / 16.0 * (15.0 - 2 * j)));
			animal_pos[i * 5 + j] = i * 5 + j;
		}
	}

	//load audio
	if (!buffer1.loadFromFile("sound/chop.wav")) {
		std::cout << "fail to load chop" << std::endl;
	}
	if (!buffer2.loadFromFile("sound/death.wav")) {
		std::cout << "fail to load death" << std::endl;
	}

	sound1.setBuffer(buffer1);
	sound2.setBuffer(buffer2);

	//load font
	if (!font.loadFromFile("fonts/KOMIKAP_.ttf")) {
		std::cout << "fail to load font" << std::endl;
	}
	text.setFont(font);	

	//load background
	if (!background_texture.loadFromFile("graphics/background.png")) {
		std::cout << "fail to load background" << std::endl;
	}
	background.setTexture(background_texture);

	//load character
	if (!buzz_symbol_texture.loadFromFile("graphics/buzzy_life.png")) {
		std::cout << "fail to load buzzy symbol" << std::endl;
	}
	buzz_symbol_texture.setSmooth(true);
	buzz_symbol.setTexture(buzz_symbol_texture);
	buzz_symbol.setOrigin(sf::Vector2f(buzz_symbol.getLocalBounds().width / 2,
		buzz_symbol.getLocalBounds().height / 2));
	buzz_symbol.scale(sf::Vector2f(0.5f, 0.5f));
	buzz_symbol.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0));

	//load buzzy
	if (!small_buzzy_texture.loadFromFile("graphics/smallbuzzy.png")) {
		std::cout << "fail to load small buzzy" << std::endl;
	}
	small_buzzy_texture.setSmooth(true);
	small_buzzy.setTexture(small_buzzy_texture);
	small_buzzy.setOrigin(sf::Vector2f(small_buzzy.getLocalBounds().width / 2,
		small_buzzy.getLocalBounds().height / 2));
	buzzy_initial = sf::Vector2f(80.f, 540.f);
	buzzy_pos = buzzy_initial;
	small_buzzy.setPosition(buzzy_pos);
	small_buzzy.setRotation(buzzy_ang);


	//load unicorn
	if (!angry_unicorn_texture.loadFromFile("graphics/angry_unicorn.png")) {
		std::cout << "fail to load angry unicorn" << std::endl;
	}
	angry_unicorn_texture.setSmooth(true);
	angry_unicorn.setTexture(angry_unicorn_texture);
	angry_unicorn.setOrigin(sf::Vector2f(angry_unicorn.getLocalBounds().width / 2,
		angry_unicorn.getLocalBounds().height / 2));
	angry_unicorn.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 7.0));

	//load frog
	if (!frog_texture.loadFromFile("graphics/frog.png")) {
		std::cout << "fail to load frog" << std::endl;
	}
	frog_texture.setSmooth(true);
	frog.setTexture(frog_texture);
	frog.setOrigin(sf::Vector2f(frog.getLocalBounds().width / 2,
		frog.getLocalBounds().height / 2));
	frog.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 9.0));

	//load bulldog
	if (!bulldog_texture.loadFromFile("graphics/Georgia_Bulldogs_logo_dog_g.png")) {
		std::cout << "fail to load bulldog" << std::endl;
	}
	bulldog_texture.setSmooth(true);
	bulldog.setTexture(bulldog_texture);
	bulldog.setOrigin(sf::Vector2f(bulldog.getLocalBounds().width / 2,
		bulldog.getLocalBounds().height / 2));
	bulldog.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 11.0));

	//load pig
	if (!pig_texture.loadFromFile("graphics/pig.png")) {
		std::cout << "fail to load pig" << std::endl;
	}
	pig_texture.setSmooth(true);
	pig.setTexture(pig_texture);
	pig.setOrigin(sf::Vector2f(pig.getLocalBounds().width / 2,
		pig.getLocalBounds().height / 2));
	pig.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 13.0));

	//load chicken
	if (!chicken_texture.loadFromFile("graphics/chicken.png")) {
		std::cout << "fail to load chicken" << std::endl;
	}
	chicken_texture.setSmooth(true);
	chicken.setTexture(chicken_texture);
	chicken.setOrigin(sf::Vector2f(chicken.getLocalBounds().width / 2,
		chicken.getLocalBounds().height / 2));
	chicken.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 15.0));

	//load sheep
	if (!sheep_texture.loadFromFile("graphics/sheep.png")) {
		std::cout << "fail to load sheep" << std::endl;
	}
	sheep_texture.setSmooth(true);
	sheep.setTexture(sheep_texture);
	sheep.setOrigin(sf::Vector2f(sheep.getLocalBounds().width / 2,
		sheep.getLocalBounds().height / 2));
	sheep.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 7.0));

	//load bunny
	if (!bunny_texture.loadFromFile("graphics/bunny.png")) {
		std::cout << "fail to load bunny" << std::endl;
	}
	bunny_texture.setSmooth(true);
	bunny.setTexture(bunny_texture);
	bunny.setOrigin(sf::Vector2f(bunny.getLocalBounds().width / 2,
		bunny.getLocalBounds().height / 2));
	bunny.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 9.0));

	//load dog
	if (!dog_texture.loadFromFile("graphics/dog.png")) {
		std::cout << "fail to load dog" << std::endl;
	}
	dog_texture.setSmooth(true);
	dog.setTexture(dog_texture);
	dog_texture.setSmooth(true);
	dog.setOrigin(sf::Vector2f(dog.getLocalBounds().width / 2,
		dog.getLocalBounds().height / 2));
	dog.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 11.0));
	

	//load tiger
	if (!tiger_texture.loadFromFile("graphics/tiger.png")) {
		std::cout << "fail to load tiger" << std::endl;
	}
	tiger_texture.setSmooth(true);
	tiger.setTexture(tiger_texture);
	tiger.setOrigin(sf::Vector2f(tiger.getLocalBounds().width / 2,
		tiger.getLocalBounds().height / 2));
	tiger.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 13.0));
	tiger.setScale(0.1f, 0.1f);

	//load mouse
	if (!mouse_texture.loadFromFile("graphics/mouse.png")) {
		std::cout << "fail to load mouse" << std::endl;
	}
	mouse_texture.setSmooth(true);
	mouse.setTexture(mouse_texture);
	mouse.setOrigin(sf::Vector2f(mouse.getLocalBounds().width / 2,
		mouse.getLocalBounds().height / 2));
	mouse.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 15.0));

	//load insect
	if (!insect_texture.loadFromFile("graphics/insect.png")) {
		std::cout << "fail to load insect" << std::endl;
	}
	insect_texture.setSmooth(true);
	insect.setTexture(insect_texture);
	insect.setOrigin(sf::Vector2f(insect.getLocalBounds().width / 2,
		insect.getLocalBounds().height / 2));
	insect.setPosition(sf::Vector2f(win_w / 16.0 * 13.0, win_h / 16.0 * 5.0));

	//load cloud
	if (!cloud_texture.loadFromFile("graphics/cloud.png")) {
		std::cout << "fail to load cloud" << std::endl;
	}
	cloud_texture.setSmooth(true);
	cloud.setTexture(cloud_texture);
	cloud.setOrigin(sf::Vector2f(cloud.getLocalBounds().width / 2,
		cloud.getLocalBounds().height / 2));
	cloud.setPosition(sf::Vector2f(win_w / 16.0 * 15.0, win_h / 16.0 * 5.0));

	animal_pointer.push_back(&chicken);
	animal_pointer.push_back(&pig);
	animal_pointer.push_back(&bulldog);
	animal_pointer.push_back(&frog);
	animal_pointer.push_back(&angry_unicorn);
	animal_pointer.push_back(&mouse);
	animal_pointer.push_back(&tiger);
	animal_pointer.push_back(&dog);
	animal_pointer.push_back(&bunny);
	animal_pointer.push_back(&sheep);

	for (int i = 0; i < 10; i++) {
		animal_pointer[i]->scale(sf::Vector2f(1.4f, 1.4f)); 
	}
}

//draw text
void buzzy_revenge::draw_text(const std::string& text_string, int size, sf::Color color, sf::Vector2f position) {
	text.setString(text_string);
	text.setStyle(sf::Text::Bold);
	text.setCharacterSize(size);
	text.setOrigin(sf::Vector2f(text.getLocalBounds().width / 2.0, text.getLocalBounds().height / 2.0));
	text.setFillColor(color);
	text.setPosition(position);
	window.draw(text);
}

//draw power bar
void buzzy_revenge::power_bar() {
	sf::RectangleShape h_line(sf::Vector2f(win_w / 6.0, 5.f));
	sf::RectangleShape v_line(sf::Vector2f(5.f, win_h / 25.0));
	sf::RectangleShape bar(sf::Vector2f(win_w / 6.0 * power, win_h / 25.0));
	h_line.setFillColor(sf::Color::Black);
	v_line.setFillColor(sf::Color::Black);
	bar.setFillColor(sf::Color::Red);
	bar.setOrigin(sf::Vector2f(0.f, bar.getLocalBounds().height / 2.0));
	bar.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0 * 23.0));
	window.draw(bar);
	h_line.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0 * 22.5));
	window.draw(h_line);
	h_line.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0 * 23.5));
	window.draw(h_line);
	v_line.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0 * 22.5));
	window.draw(v_line); 
	v_line.setPosition(sf::Vector2f(win_w / 25.0 * 2 + win_w / 6.0, win_h / 25.0 * 22.5));
	window.draw(v_line);
}

//draw life symbols
void buzzy_revenge::draw_lives() {
	buzz_symbol.setPosition(sf::Vector2f(win_w / 25.0 * 2, win_h / 25.0));
	for (int i = 0; i < lives; i++) {
		window.draw(buzz_symbol);
		buzz_symbol.move(sf::Vector2f(win_w / 25.0, 0.f));
	}
}

//update buzzy sprite
void buzzy_revenge::update_buzzy() {
	small_buzzy.setPosition(buzzy_pos);
	small_buzzy.setRotation(buzzy_ang);
}

//collect the keyboard event
void buzzy_revenge::key_action() {
	if (!lock_key) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			if (!key_pressed) {
				clock.restart();
				key_pressed = true;
				pressing_key = sf::Keyboard::Up;
			}
			else {
				if (pressing_key == sf::Keyboard::Up) {
					elapsed = clock.restart();
					buzzy_ang -= ang_vel * elapsed.asSeconds();
					if (buzzy_ang < -90) {
						buzzy_ang = -90;
					}
				}
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			if (!key_pressed) {
				clock.restart();
				key_pressed = true;
				pressing_key = sf::Keyboard::Down;
			}
			else {
				if (pressing_key == sf::Keyboard::Down) {
					elapsed = clock.restart();
					buzzy_ang += ang_vel * elapsed.asSeconds();
					if (buzzy_ang > 90) {
						buzzy_ang = 90;
					}
				}
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (!key_pressed) {
				clock.restart();
				key_pressed = true;
				pressing_key = sf::Keyboard::Space;
			}
			else {
				if (pressing_key == sf::Keyboard::Space) {
					elapsed = clock.restart();
					power += power_acc * elapsed.asSeconds();
					if (power > 1) {
						power = 1;
					}
					vi = power * k + min;
				}
			}
		}
		else {
			if (pressing_key == sf::Keyboard::Space) {
				lock_key = true;
				buzz_launch = true;
				firing_ang = buzzy_ang;
			}
			key_pressed = false;
			pressing_key = sf::Keyboard::Unknown;
		}
	}
}

//use to initiate buzzy
void buzzy_revenge::restart_buzzy() {
	buzzy_ang = initial_ang;
	buzzy_pos = buzzy_initial;
	power = 0;
	firing_ang = initial_ang;
	vi = min;
}

//draw the auxiliary line for aiming assistance
void buzzy_revenge::auxiliary() {
	sf::CircleShape pt(win_w / 500.0);
	pt.setFillColor(sf::Color(255, 255, 255, 128));
	pt.setOrigin(sf::Vector2f(pt.getLocalBounds().width / 2.0, pt.getLocalBounds().height / 2.0));
	float x, y,interval;
	interval = 0.15;
	if (!lock_key) {
		for (int i = 0; i < 20; i++) {
			//std::cout << "buzzy angle " << buzzy_ang << std::endl;
			x = buzzy_initial.x + interval * i * vi * std::cos(buzzy_ang / 180.0 * 3.1415926535897932);
			y = buzzy_initial.y + interval * i * vi * std::sin(buzzy_ang / 180.0 * 3.1415926535897932) +
				0.5 * gravity * interval * i * interval * i;
			pt.setPosition(sf::Vector2f(x, y));
			window.draw(pt);
		}
	}
}

//draw the projectile of buzzy
void buzzy_revenge::projectile() {
	int num;
	if (buzz_launch) {
		clock.restart();
		buzz_launch = false;
		vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
		vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0);
	}
	if (lock_key) {
		if (!isbouncing) {
			elapsed = clock.getElapsedTime();
			buzzy_pos.x = buzzy_initial.x + elapsed.asSeconds() * vi * std::cos(firing_ang / 180.0 * 3.1415926535897932);
			buzzy_pos.y = buzzy_initial.y + elapsed.asSeconds() * vi * std::sin(firing_ang / 180.0 * 3.1415926535897932) +
				0.5 * gravity * elapsed.asSeconds() * elapsed.asSeconds();
			//float vy = vi * std::sin(firing_ang / 180.0 * 3.1415926535897932) + gravity * elapsed.asSeconds();
			buzzy_ang = atan2(vi * std::sin(firing_ang / 180.0 * 3.1415926535897932) + gravity * elapsed.asSeconds(),
				vi * std::cos(firing_ang / 180.0 * 3.1415926535897932)) * 180.0 / 3.1415926535897932;
			num = crush();
			if (num == 0) {
				isbouncing = true;
				hit_mascot();
				vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
				elapsed = clock.restart();
				vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0) + gravity * elapsed.asSeconds();
				vy = -vy;
			}
			if (num == 1) {
				isbouncing = true;
				hit_mascot();
				vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
				vx = -vx;
				elapsed = clock.restart();
				vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0) + gravity * elapsed.asSeconds();
			}
			if (num == 2) {
				isbouncing = true;
				if(unicorn == -1){
					//hit_woodland_creature();
				}
				vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
				elapsed = clock.restart();
				vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0) + gravity * elapsed.asSeconds();
				vy = -vy;
			}
			if (num == 3) {
				isbouncing = true;
				if (unicorn == -1) {
					//hit_woodland_creature();
				}
				vx = vi * cos(firing_ang * 3.1415926535897932 / 180.0);
				vx = -vx;
				elapsed = clock.restart();
				vy = vi * sin(firing_ang * 3.1415926535897932 / 180.0) + gravity * elapsed.asSeconds();
			}
		}
		else {
			elapsed = clock.restart();
			buzzy_pos.x += elapsed.asSeconds() * vx;
			vy += gravity * elapsed.asSeconds();
			buzzy_pos.y += elapsed.asSeconds() * vy;
			buzzy_ang += spin * elapsed.asSeconds();
		}
		//std::cout << "vx " << " " << vi * std::cos(firing_ang / 180.0 * 3.1415926535897932) << std::endl;
		
		if (buzzy_pos.x > win_w || buzzy_pos.x < 0 || buzzy_pos.y > win_h) {
			if (!ishit) {
				lives--;
			}
			else {
				ishit = false;
			}
			restart_buzzy();
			lock_key = false;
			isbouncing = false;
		}
	}
}

//use to determine whether the buzzy are colliding with others
int buzzy_revenge::crush() {
	float num;
	float distance;
	for (int i = 0; i < 10; i++) {
		if (animal_pos[i] != -1) {
			distance = sqrt((buzzy_pos.x - all_pos[i].x) * (buzzy_pos.x - all_pos[i].x)
				+ (buzzy_pos.y - all_pos[i].y) * (buzzy_pos.y - all_pos[i].y));
			if (distance < win_h / 10.0) {
				sound1.play();
				if (i < 5) {
					num = 15.5;
				}
				else {
					num = 14;
				}
				if (animal_pos[i] == 2 || animal_pos[i] == 6) {
					ishit = true;
					if (i < 5) {
						hit_column = 2;
					}
					else {
						hit_column = 1;
					}
					fall_clock.restart();
					if (buzzy_pos.x > win_w / 16.0 * num) {
						return 0;
					}
					else {
						return 1;
					}
				}
				else {
					if (animal_pos[i] == 4) {
						unicorn = i;
						lives = 5;
						ishit = true;
						fall_clock.restart();
					}
					if (buzzy_pos.x > win_w / 16.0 * num) {
						return 2;
					}
					else {
						return 3;
					}
				}
			}
		}
	}

	if (insect_appear) {
		distance = sqrt((buzzy_pos.x - insect_pos.x) * (buzzy_pos.x - insect_pos.x)
			+ (buzzy_pos.y - insect_pos.y) * (buzzy_pos.y - insect_pos.y));
		if (distance < win_h / 10.0) {
			sound1.play();
			score += 75;
			insect_appear = false;
			ishit = true;
		}
	}
	return -1;
}

void buzzy_revenge::hit_woodland_creature() {
	lives--;
}


void buzzy_revenge::hit_mascot() {
	score += 25;
}

//use to draw a column falling
void buzzy_revenge::column_fall() {
	if (hit_column == 1) {
		fall_time = fall_clock.restart();
		for (int i = 5; i < 10; i++) {
			if (animal_pos[i] < 0) {
				continue;
			}
			//std::cout << "falling " << fall_time.asSeconds() << std::endl;
			animal_pointer[animal_pos[i]]->move(sf::Vector2f(0.f, fall_speed * fall_time.asSeconds()));
			animal_pointer[animal_pos[i]]->rotate(spin * fall_time.asSeconds());
			window.draw(*animal_pointer[animal_pos[i]]);
		}
		for (int i = 0; i < 5; i++) {
			if (animal_pos[i] < 0) {
				continue;
			}
			window.draw(*animal_pointer[animal_pos[i]]);
		}
		int i = 9;
		while (animal_pos[i] < 0) {
			i--;
			if (i == 5) break;
		}
		if (animal_pointer[animal_pos[i]]->getPosition().y > win_h * 9.0 / 8.0) {
			for (int i = 5; i < 10; i++) {
				animal_pos[i] = -1;
			}
			hit_column = 0;
		}
	}
	if (hit_column == 2) {
		fall_time = fall_clock.restart();
		for (int i = 0; i < 5; i++) {
			if (animal_pos[i] < 0) {
				continue;
			}
			//std::cout << "falling " << fall_time.asSeconds() << std::endl;
			animal_pointer[animal_pos[i]]->move(sf::Vector2f(0.f, fall_speed * fall_time.asSeconds()));
			animal_pointer[animal_pos[i]]->rotate(spin * fall_time.asSeconds());
			window.draw(*animal_pointer[animal_pos[i]]);
		}
		for (int i = 5; i < 10; i++) {
			if (animal_pos[i] < 0) {
				continue;
			}
			window.draw(*animal_pointer[animal_pos[i]]);
		}
		int i = 4;
		while (animal_pos[i] < 0) {
			i--;
			if (i == 0) break;
		}
		if (animal_pointer[animal_pos[i]]->getPosition().y > win_h * 9.0 / 8.0) {
			for (int i = 0; i < 5; i++) {
				animal_pos[i] = -1;
			}
			hit_column = 0;
		}
	}
}

//use when unicorn are hit and the images above fall
void buzzy_revenge::unicorn_fall() {
	if (unicorn >= 5) {
		fall_time = fall_clock.restart();
		animal_pos[unicorn] = -1;
		for (int i = 0; i < 10; i++) {
			//std::cout << "falling " << fall_time.asSeconds() << std::endl;
			if (animal_pos[i] < 0) {
				continue;
			}
			if (i > unicorn) {
				animal_pointer[animal_pos[i]]->move(sf::Vector2f(0.f, fall_speed * fall_time.asSeconds()));
				//animal_pointer[animal_pos[i]]->rotate(spin * fall_time.asSeconds());
			}
			window.draw(*animal_pointer[animal_pos[i]]);
		}

		if (animal_pos[9] >= 0) {
			//std::cout << "distance " << abs(animal_pointer[animal_pos[9]]->getPosition().y - all_pos[8].y) << std::endl;
			if (abs(animal_pointer[animal_pos[9]]->getPosition().y - all_pos[8].y) < 2) {
				for (int i = unicorn; i < 9; i++) {
					animal_pos[i] = animal_pos[i + 1];
				}
				animal_pos[9] = -1;
				unicorn = -1;
			}
		}
		else {
			unicorn = -1;
		}
	}
	if (unicorn < 5 && unicorn >= 0) {
		fall_time = fall_clock.restart();
		animal_pos[unicorn] = -1;
		for (int i = 0; i < 10; i++) {
			//std::cout << "falling " << fall_time.asSeconds() << std::endl;
			if (animal_pos[i] < 0) {
				continue;
			}
			if (i > unicorn && i <= 4) {
				animal_pointer[animal_pos[i]]->move(sf::Vector2f(0.f, fall_speed * fall_time.asSeconds()));
				//animal_pointer[animal_pos[i]]->rotate(spin * fall_time.asSeconds());
			}
			window.draw(*animal_pointer[animal_pos[i]]);
		}

		if (animal_pos[4] >= 0) {
			//std::cout << "distance " << abs(animal_pointer[animal_pos[4]]->getPosition().y - all_pos[3].y) << std::endl;
			if (abs(animal_pointer[animal_pos[4]]->getPosition().y - all_pos[3].y) < 2) {
				for (int i = unicorn; i < 4; i++) {
					animal_pos[i] = animal_pos[i + 1];
				}
				animal_pos[4] = -1;
				unicorn = -1;
			}
		}
		else {
			unicorn = -1;
		}
	}
}

//use to draw animal
void buzzy_revenge::draw_animal() {
	for (int i = 0; i < 10; i++) {
		if (animal_pos[i] >= 0) {
			//std::cout << i << " ";
			animal_pointer[animal_pos[i]]->setPosition(all_pos[i]);
			window.draw(*animal_pointer[animal_pos[i]]);
		}
	}
	//std::cout << std::endl;
}

//create randomly arranged animal columns
void buzzy_revenge::random_animal() {
	std::vector<int> pool;
	int idx;
	int count = 0;

	for (int i = 0; i < 10; i++) {
		animal_pos[i] = -1;
		pool.push_back(i);
	}

	if (rand() % 2) {
		//tiger in column 2
		idx = rand() % 5 + 5;
		animal_pos[idx] = 6;
		pool.erase(pool.begin() + 6);
		idx = rand() % 5;
		animal_pos[idx] = 2;
		pool.erase(pool.begin() + 2);
		for (int i = 0; i < 10; i++) {
			/*
			std::cout << "pool ";
			for (auto iter = pool.begin(); iter != pool.end(); iter++) {
				std::cout << *iter << " ";
			}
			std::cout << std::endl;
			*/
			if (animal_pos[i] == -1) {
				idx = rand() % (8 - count);
				animal_pos[i] = pool[idx];
				pool.erase(pool.begin() + idx);
				count++;
			}
		}
	}
	else {
		//tiger in column 1
		idx = rand() % 5;
		animal_pos[idx] = 6;
		pool.erase(pool.begin() + 6);
		idx = rand() % 5 + 5;
		animal_pos[idx] = 2;
		pool.erase(pool.begin() + 2);
		for (int i = 0; i < 10; i++) {
			if (animal_pos[i] == -1) {
				idx = rand() % (8 - count);
				animal_pos[i] = pool[idx];
				pool.erase(pool.begin() + idx);
				count++;
			}
		}
	}
	/*
	for (int i = 0; i < 10; i++) {
		std::cout << animal_pos[i] << " ";
	}
	std::cout << std::endl;
	*/
}

//use to draw and update the position of insect
void buzzy_revenge::update_insect() {
	if (insect_ready) {
		insect_pos = insect_initpos;
		insect_vy = 0;
		insect_clock.restart();
		insect_ready = false;
		insect_lock = true;
	}
	insect_time = insect_clock.getElapsedTime();
	if (insect_time.asSeconds() > 5.0 && insect_lock) {
		insect_lock = false;
		insect_appear = true;
		insect_clock.restart();
		insect.setPosition(insect_pos);
		window.draw(insect);
		vary_count = 0;
	}
	if (insect_appear) {
		vary_count++;
		if (vary_count == 300) {
			vary_count = 0;
			insect_acc = rand() % insect_max_acc - insect_max_acc / 2.0;
		}
		insect_time = insect_clock.restart();
		insect_vy += insect_time.asSeconds() * (insect_acc);
		insect_pos.x += insect_vx * insect_time.asSeconds();
		insect_pos.y += insect_vy * insect_time.asSeconds();
		insect.setPosition(insect_pos);
		window.draw(insect);
		if (insect_pos.y < win_h / 16.0) {
			insect_vy = abs(insect_vy);
		}
		if (insect_pos.y > win_h / 16.0 * 15.0) {
			insect_vy = -abs(insect_vy);
		}
		if (insect_pos.x > win_w / 16.0 * 17.0) {
			insect_appear = false;
		}
	}

}

//check if the columns are all eliminated
void buzzy_revenge::check_level_up() {
	bool level_up = true;
	for (int i = 0; i < 10; i++) {
		if (animal_pos[i] > 0) {
			level_up = false;
			//std::cout << " " << animal_pos[i] << " ";
		}
	}
	//std::cout << level_up << std::endl;
	if (level_up) {
		restart_level();
	}
}

//restart the level when eliminate all columns
void buzzy_revenge::restart_level() {
	restart_buzzy();
	random_animal();
	for (int i = 0; i < 10; i++) {
		animal_pointer[i]->setRotation(0);
	}
	insect_ready = true;
	insect_appear = false;
}

//use to restart the game when ESC or running out of lives
void buzzy_revenge::restart_game() {
	restart_level();
	score = 0;
	lives = 5;
	hit_column = 0;
	unicorn = -1;
	isbouncing = false;
	isfalling = false;
	ishit = false;
}

//draw the start menu
void buzzy_revenge::draw_start_menu() {
	bool start_game = false;
	//window.create(sf::VideoMode(win_w, win_h), "Buzzy's Revenge");
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			break;
		}
		
		window.clear();
		window.draw(background);
		draw_lives();
		window.draw(small_buzzy);
		window.draw(angry_unicorn);
		window.draw(frog);
		window.draw(bulldog);
		window.draw(pig);
		window.draw(chicken);
		window.draw(sheep);
		window.draw(bunny);
		window.draw(dog);
		window.draw(tiger);
		window.draw(mouse);
		window.draw(insect);
		draw_text("Buzzy's Revenge", 72, sf::Color::Red, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 3.0));
		draw_text("Press Enter to Restart Game", 60, sf::Color::White, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 6.0));
		draw_text("Press Esc to exit", 60, sf::Color::White, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 7.0));
		draw_text("Press Space to Powerup", 60, sf::Color::White, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 8.0));
		draw_text("Press up/down arrow to aim", 60, sf::Color::White, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 9.0));
		draw_text("Created by\nZhaodong Yang", 24, sf::Color::White, sf::Vector2f(win_w / 2.0, win_h / 14.0 * 12.0));
		draw_text("Lives", 24, sf::Color::White, sf::Vector2f(win_w / 25.0, win_h / 25.0));
		draw_text("Score:" + std::to_string(score), 24, sf::Color::White, sf::Vector2f(win_w / 25.0 * 23.0, win_h / 25.0));
		draw_text("Power", 24, sf::Color::White, sf::Vector2f(win_w / 25.0, win_h / 25.0 * 23.0));
		power = 1.0;
		power_bar();
		power = 0.0;
		window.display();
		
	}
}

//draw game GUI
void buzzy_revenge::draw_game_gui() {
	buzz_launch = false;
	lock_key = false;
	key_pressed = false;
	pressing_key = sf::Keyboard::Unknown;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			restart_game();
		}

		if (lives == 0) {
			sound2.play();
			draw_start_menu();
			restart_game();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
			draw_start_menu();
			restart_game();
		}

		check_level_up();

		key_action();

		//motion function of buzzy
		projectile();

		window.clear();
		//background
		window.draw(background);
		//auxiliary line for aiming
		auxiliary();
		//lives
		draw_lives();
		//draw small buzzy
		update_buzzy();
		window.draw(small_buzzy);

		//draw animal
		column_fall();
		unicorn_fall();
		if (hit_column == 0 && unicorn == -1) {
			draw_animal();
		}


		update_insect();
		draw_text("Lives", 24, sf::Color::White, sf::Vector2f(win_w / 25.0, win_h / 25.0));
		draw_text("Score:" + std::to_string(score), 24, sf::Color::White, sf::Vector2f(win_w / 25.0 * 23.0, win_h / 25.0));
		draw_text("Power", 24, sf::Color::White, sf::Vector2f(win_w / 25.0, win_h / 25.0 * 23.0));
		power_bar();
		window.display();
	}
}

int main() 
{
	buzzy_revenge game;
	game.draw_start_menu();
	game.draw_game_gui();
}


