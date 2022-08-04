#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include "math.hpp"

struct {
	float posX;
	float posY;
	float dX;
	float dY;
	float dir;
	float dSteps;
	void moveSteps(float steps) {
	// if (steps > 0)
	// 	{
	// 	posX += sin(degToRad(dir)*steps)/25;
	// 	posY += cos(degToRad(dir)*steps)/25;
	// }
	// else
	// {
		posX += (cos(degToRad(dir))*steps)/25;
		posY += (sin(degToRad(dir))*steps)/25;
	// }

	}
	void pointTowardsVector2f(sf::Vector2<float> vector) {
		float larger = -1.0f;
		if (vector.y < posY)
			larger = 1.0f;
		dir = atan(((vector.x-posX)/(vector.y-posY))+(180.0f*larger));
	}
	sf::Vector2f getPosition() 
	{
		return sf::Vector2f(posX,posY);
	}

} player;

struct raycastResult {
	float steps;
	sf::Vector2f result;
};

sf::CircleShape shape(15,3);


std::vector<sf::RectangleShape> walls;


bool isCollidingWithWall(sf::Vector2f point, sf::RectangleShape wall) {
	sf::Vector2f size = wall.getSize();
	return point.x > size.x && point.y > size.x;
}
bool shapeCollision(sf::CircleShape shape1, sf::RectangleShape shape2) {
	sf::Rect<float> box1 = shape1.getGlobalBounds();
	sf::Rect<float> box2 = shape2.getGlobalBounds();
	return box1.intersects(box2);
}
std::vector<sf::RectangleShape> playerCollidesWithWall() {
	std::vector<sf::RectangleShape> collidingwalls;
	for (auto i : walls)
	{
		if (shapeCollision(shape, i) ){
			collidingwalls.push_back(i);
		}
	}
	return collidingwalls;
}

raycastResult raycast(sf::Vector2f origin, float angle, float maxLength, sf::RenderWindow &window) {
	// returns point of intersection
	float x = origin.x;
	float y = origin.y;
	sf::RectangleShape rect(sf::Vector2<float>(5,5));
	raycastResult toReturn;

	bool br = false;

	for (int i = 0; i < maxLength; ++i) {
		for (auto wall : walls)
		{
			sf::Vector2f size = wall.getSize();
			sf::Vector2f position = wall.getPosition();

			if (
			x > position.x + size.x/2 && x < position.x - size.x/2 &&
			x > position.y + size.y/2 && x < position.y - size.y/2  
			)
			{
				toReturn.result = sf::Vector2<float>(x,y);
				toReturn.steps = i;
				br = true;
				std::cout << "WE INTERSECTED BABYY";
				break;
			}
		}
		x += (cos(degToRad(angle)))/25;
		y += (sin(degToRad(angle)))/25;
		rect.setPosition(x,y);
		window.clear();
		window.draw(rect);
		window.display();
		if (br)
			break;
	}
	if (!br)
		toReturn.steps = -1.0f;
	return toReturn;
}

void addWall(sf::Vector2f size, sf::Vector2f position) {
	sf::RectangleShape newWall(size);
	newWall.setPosition(position);
	walls.push_back(newWall);
}

void renderWalls(sf::RenderWindow &window) {
	for (auto i : walls)
	{
		window.draw(i);
	}
}

void updatePlayer() {
	shape.setPosition(sf::Vector2<float>(player.posX+player.dX,player.posY+player.dY));
	player.moveSteps(player.dSteps);
}

int main(int argc, char* argv[]) {
	// init player
	player.posX = 512/2;
	player.posY = 512/2;
	player.dX = 0.0f;
	player.dY = 0.0f;
	player.dSteps = 0.0f;
	player.dir = 0.0f;
	shape.setOrigin(shape.getRadius(), shape.getRadius());

	// create walls
	addWall(sf::Vector2<float>(25,50),sf::Vector2<float>(300,300));
	// create window
	sf::RenderWindow window(sf::VideoMode(512, 512), "new window",sf::Style::Default);

	// main window loop
	while (window.isOpen()) 
	{

		sf::Event evnt;
        while (window.pollEvent(evnt))
        {
        	// every time something gets pressed
            if (evnt.type == sf::Event::Closed) 
            {
                window.close();
            } 
        }
        // while keys are pressed thing
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    		player.dSteps = -3.0f;

    	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    		player.dSteps = 3.0f;
    	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))

    		player.dir -= 0.3f;
    		if (player.dir - 0.1f <= -360) {
    			player.dir = 0;
    		}
    		shape.setRotation(player.dir);

    	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))

    		player.dir += 0.3f;
    	    if (player.dir + 0.1f >= 360) {
    			player.dir = 0;
    		}
    		shape.setRotation(player.dir);
    		// when none of the direction keys are pressed 
    	if ((
    		!(sf::Keyboard::isKeyPressed(sf::Keyboard::W))) && 
    		(!(sf::Keyboard::isKeyPressed(sf::Keyboard::A))) && 
    		(!(sf::Keyboard::isKeyPressed(sf::Keyboard::S))) && 
    		(!(sf::Keyboard::isKeyPressed(sf::Keyboard::D))) 
    		)
    		player.dSteps *= 0.997f;
        updatePlayer();
        std::vector<sf::RectangleShape> intersecting = playerCollidesWithWall();
        if (intersecting.size() != 0) {
        	while (intersecting.size() != 0) {
        		if (playerCollidesWithWall().size() <= 0)
        			break;
        		player.moveSteps(1.0f);
        		player.posX += (cos(degToRad(player.dir))*2.0f)/25;
				player.posY += (sin(degToRad(player.dir))*2.0f)/25;
				player.dSteps = 0.0f;
        		updatePlayer();
        	}
        	
        }
        std::cout << player.posX << " " << player.posY << std::endl;
        raycastResult result = raycast(sf::Vector2<float>(player.posX,player.posY),player.dir,10,window);
		renderWalls(window);
		window.display();
	}
	return 0;
}