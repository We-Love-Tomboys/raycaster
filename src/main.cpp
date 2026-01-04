#include <SFML/Graphics.hpp>
#include <iostream>
sf::Vector2f tile2Screen(sf::Vector2f tile, float tileSize) {
	float x = static_cast<float>((tile.x) * tileSize);
	float y = static_cast<float>((tile.y) * tileSize);
	return sf::Vector2f({ x,y });
}

char clamp(int value, int min, int max) {
	return (value < min) ? min : (value > max) ? max : value;
}

int main() {

	int mapWidth = 8;
	int mapHeight = 8;
	int mapSize = 64;
	float tileSize = 100;
	// Each 12.5 px for 1 tile
	int map[8][8] = {
		{1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,1,1},
		{1,0,0,0,0,0,0,1},
		{1,0,0,0,0,1,1,1},
		{1,0,0,0,0,1,0,1},
		{1,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1},
	};
	// The Screen Will be divided into 2 parts
	sf::RenderWindow window(sf::VideoMode({ 1600, 800 }), "Kite Raycaster");
	auto [width, height] = window.getSize();
	float screenHeight = static_cast<float>(height);
	float screenWidth = static_cast<float>(width);
	sf::Vector2f playerPostion({ static_cast<float>(width / 4), static_cast<float>(height / 2) }); // Set Player on the Center of the screen
	sf::RectangleShape player({ 10.f, 20.f });
	player.setOrigin({ 5.f, 5.f });
	float playerRotation = 0.f;
	float playerSpeed = 300.f;
	float playerRotationSpeed = 5.f;

	sf::RectangleShape divider({ 1.f, static_cast<float>(height) });
	divider.setPosition({ static_cast<float>(width / 2.f), 0.f });

	sf::Clock clock;

	float maxDistance = 20000;
	float fov = 90 * (3.14 / 180.f); // Radians
	int numRays = 120;
	float stepDistance = 1.f;

	while (window.isOpen()) {
		window.clear(sf::Color::Black);

		float deltaTime = clock.restart().asSeconds();
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			playerRotation -= playerRotationSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			playerRotation += playerRotationSpeed * deltaTime;
		}
		sf::Vector2f forward(
			std::sin(playerRotation),
			-std::cos(playerRotation)
		);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			playerPostion.x += forward.x * playerSpeed * deltaTime;
			playerPostion.y += forward.y * playerSpeed * deltaTime;
		}

		for (int i = 0; i < numRays; i++) {
			// We need to calculate the angle from the left to the right with corresponding the value of `i`
			//float rayAngle = (fov / 2) + playerRotation + i;
			float rayAngle = playerRotation - fov / 2 + fov * i / numRays;

			sf::Vector2f rayPosition = playerPostion;
			sf::Vector2f rayDirection(std::sin(rayAngle), -std::cos(rayAngle));
			float distance = 0;
			while (distance < maxDistance) {
				rayPosition += rayDirection * stepDistance;
				distance += stepDistance;
				int mapX = static_cast<int>(rayPosition.x / tileSize);
				int mapY = static_cast<int>(rayPosition.y / tileSize);

				if (map[mapY][mapX] == 1)
					break;
			}
			float lineHeight = screenHeight / (distance / tileSize);
			short depth = static_cast<int>(distance); // more distance more dark
			depth = clamp(depth, 0, 255);
			sf::RectangleShape slice({ (screenWidth) / numRays, lineHeight });
				
			slice.setFillColor(sf::Color(255, 255, 255, static_cast<char>(255 - depth)));
			slice.setPosition({ i * (screenWidth / float(numRays)), (screenHeight - lineHeight)/2 });
			window.draw(slice);
		}


		player.setPosition(playerPostion);
		player.setRotation(sf::radians(playerRotation));

		for (int i = 0; i < mapWidth; i++) {
			for (int j = 0; j < mapWidth; j++) {
				// Convert the i, j from the tile to screen coordinates
				if (map[j][i] == 1) {
					sf::RectangleShape tile({ tileSize, tileSize });
					tile.setPosition(tile2Screen(sf::Vector2f({ static_cast<float>(i),static_cast<float>(j) }), tileSize));
					//window.draw(tile);
				}
			}
		}

		/*window.draw(player);
		window.draw(divider);*/
		window.display();
	}
}