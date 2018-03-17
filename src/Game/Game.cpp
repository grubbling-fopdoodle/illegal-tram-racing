#include "Game.h"

void Game::Start()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.antialiasingLevel = 4;

	window.create(
		sf::VideoMode(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT),
		Constants::CurrentText().GetText(LocalizedText::TextType::WINDOW_TITLE),
		sf::Style::Default,
		settings
		);
	window.setActive(true);

	resources.Initialize();

	gl::glEnable(gl::GLenum::GL_DEPTH_TEST);
	
	AddObject("Cube", "", MeshId::CUBE);

	MainLoop();
}

void Game::MainLoop()
{

	bool running = true;
	while(running)
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if(event.type == sf::Event::Resized)
			{
				gl::glViewport(0, 0, event.size.width, event.size.height);
			}
			else if(event.type == sf::Event::MouseWheelScrolled)
			{
				camera.Zoom(event.mouseWheelScroll.delta * -1.0f * Constants::MOUSE_WHEEL_ZOOM_SPEED);
			}
		}

		Update();
		Render();
	}
	window.close();
}

void Game::Update()
{
	sf::Vector2i mouseOffset = sf::Mouse::getPosition() - lastMousePos;
	if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && (mouseOffset.x != 0 || mouseOffset.y != 0))
	{
		camera.RotateAround(mouseOffset.x, mouseOffset.y);
	}
	lastMousePos = sf::Mouse::getPosition();

}

void Game::Render()
{

	gl::glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	gl::glClear(gl::ClearBufferMask::GL_COLOR_BUFFER_BIT | gl::ClearBufferMask::GL_DEPTH_BUFFER_BIT);

	gl::glUseProgram(resources.GetShaderProgram(ShaderProgramId::MAIN)->GlId());

	const unsigned int mainTextureLocation = gl::glGetUniformLocation(resources.GetShaderProgram(ShaderProgramId::MAIN)->GlId(), "mainTexture");
	gl::glUniform1i(mainTextureLocation, 0);

	for (int i = 0; i < objects.size(); i++)
	{
		DrawObject(i);
	}
	window.display();
}

void Game::AddObject(std::string name, std::string parentName, MeshId meshId)
{
	Transform* parentTransform = nullptr;
	if(objectIds.find(parentName) != objectIds.end())
	{
		parentTransform = objects[objectIds[parentName]]->GetTransform();
	}
	objects.push_back(std::make_unique<GameObject>(name, parentTransform, meshId));
	objectIds[name] = objects.size() - 1;
}

void Game::DrawObject(int objectId)
{
	const unsigned int modelLocation = gl::glGetUniformLocation(resources.GetShaderProgram(ShaderProgramId::MAIN)->GlId(), "model");
	glm::mat4 modelMatrix = objects[objectId]->GetTransform()->ModelMatrix();
	gl::glUniformMatrix4fv(modelLocation, 1, gl::GL_FALSE, glm::value_ptr(modelMatrix));

	const unsigned int viewLocation = gl::glGetUniformLocation(resources.GetShaderProgram(ShaderProgramId::MAIN)->GlId(), "view");
	glm::mat4 viewMatrix = camera.ViewMatrix();
	gl::glUniformMatrix4fv(viewLocation, 1, gl::GL_FALSE, glm::value_ptr(viewMatrix));

	const unsigned int projectionLocation = gl::glGetUniformLocation(resources.GetShaderProgram(ShaderProgramId::MAIN)->GlId(), "projection");
	glm::mat4 projectionMatrix = camera.ProjectionMatrix();
	gl::glUniformMatrix4fv(projectionLocation, 1, gl::GL_FALSE, glm::value_ptr(projectionMatrix));

	gl::glActiveTexture(gl::GLenum::GL_TEXTURE0);
	gl::glBindTexture(gl::GLenum::GL_TEXTURE_2D, resources.GetTexture(TextureId::CRATE)->GlId());

	auto& mesh = resources.GetMesh(objects[objectId]->GetMeshId());
	gl::glBindVertexArray(mesh->Vao());

	gl::glPolygonMode(gl::GLenum::GL_FRONT_AND_BACK, gl::GLenum::GL_FILL);
	gl::glDrawElements(gl::GLenum::GL_TRIANGLES, mesh->ElementCount(), gl::GLenum::GL_UNSIGNED_INT, nullptr);
}
