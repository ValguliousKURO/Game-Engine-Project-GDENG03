#include "MainGame.h"
#include "Objects/Player.h"
#include <cmath>
#include <imgui.h>
#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <DX3D/Graphics/GraphicsDevice.h>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

// Simple helper function to load a Win32/DirectX11 texture from a file using stb_image
bool LoadTextureFromFile(const char* filename, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	HRESULT hr = d3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	if (FAILED(hr))
	{
		stbi_image_free(image_data);
		return false;
	}

	// Create Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	stbi_image_free(image_data);

	if (FAILED(hr))
		return false;

	*out_width = image_width;
	*out_height = image_height;

	return true;
}

MainGame::~MainGame()
{
	if (m_logoTextureSRV)
	{
		m_logoTextureSRV->Release();
		m_logoTextureSRV = nullptr;
	}
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	m_plane = world.createGameObject<dx3d::GameObject>();
	m_plane->createOrGetComponent<dx3d::PlaneComponent>();
	m_plane->getTransform().setScale({ 14.0f, 1.0f, 14.0f });
	m_plane->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });

	/*m_cubeLeft = world.createGameObject<dx3d::GameObject>();
	m_cubeLeft->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeLeft->getTransform().setScale({ 1.0f, 2.0f, 1.0f });
	m_cubeLeft->getTransform().setPosition({ -3.0f, 0.0f, 4.0f });*/

	m_teapot = world.createGameObject<dx3d::GameObject>();
	auto teapotModel = m_teapot->createOrGetComponent<dx3d::ModelComponent>();
	teapotModel->setModelType(dx3d::ModelType::Teapot);
	m_teapot->getTransform().setScale({ 2.1f, 2.1f, 2.1f });
	m_teapot->getTransform().setPosition({ -4.0f, 0.15f, 6.0f });
	m_teapot->getTransform().setRotation({ 0.0f, 0.8f, 0.0f });

	m_bunny = world.createGameObject<dx3d::GameObject>();
	auto bunnyModel = m_bunny->createOrGetComponent<dx3d::ModelComponent>();
	bunnyModel->setModelType(dx3d::ModelType::Bunny);
	bunnyModel->setTint({ 0.75f, 0.82f, 0.92f, 1.0f });
	m_bunny->getTransform().setScale({ 2.8f, 2.8f, 2.8f });
	m_bunny->getTransform().setPosition({ 0.0f, 0.05f, 6.0f });
	m_bunny->getTransform().setRotation({ 0.0f, -0.4f, 0.0f });

	m_armadillo = world.createGameObject<dx3d::GameObject>();
	auto armadilloModel = m_armadillo->createOrGetComponent<dx3d::ModelComponent>();
	armadilloModel->setModelType(dx3d::ModelType::Armadillo);
	armadilloModel->setTint({ 0.88f, 0.72f, 0.52f, 1.0f });
	m_armadillo->getTransform().setScale({ 2.6f, 2.6f, 2.6f });
	m_armadillo->getTransform().setPosition({ 4.0f, 0.1f, 6.0f });
	m_armadillo->getTransform().setRotation({ 0.0f, -0.9f, 0.0f });

	/*m_sphere = world.createGameObject<dx3d::GameObject>();
	m_sphere->createOrGetComponent<dx3d::SphereComponent>();
	m_sphere->getTransform().setScale({ 1.4f, 1.4f, 1.4f });
	m_sphere->getTransform().setPosition({ 0.0f, 1.0f, 7.5f });*/

	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 0.0f, 1.5f, -7.0f });
	m_player->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);

	// Load the logo texture
	m_logoLoaded = LoadTextureFromFile("logo.png", getGraphicsDevice().getD3DDevice(), &m_logoTextureSRV, &m_logoWidth, &m_logoHeight);
	if (!m_logoLoaded)
	{
		m_logoLoaded = LoadTextureFromFile("dlsu_logo.png", getGraphicsDevice().getD3DDevice(), &m_logoTextureSRV, &m_logoWidth, &m_logoHeight);
	}
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	m_elapsedTime += deltaTime;

	if (m_cubeLeft)
	{
		m_cubeLeft->getTransform().setRotation({ 0.0f, m_elapsedTime, 0.0f });
	}

	if (m_cubeRight)
	{
		m_cubeRight->getTransform().setRotation({ m_elapsedTime * 0.45f, 0.7f, m_elapsedTime * 0.25f });
	}

	if (m_sphere)
	{
		const auto scale = 1.25f + (std::sin(m_elapsedTime * 1.5f) * 0.2f);
		m_sphere->getTransform().setScale({ scale, scale, scale });
	}
}

void MainGame::onRenderUI()
{
	// 1. Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				PostQuitMessage(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Scene Hierarchy", nullptr, &m_showHierarchy);
			ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Color Picker", nullptr, &m_showColorPicker);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About"))
		{
			ImGui::MenuItem("Credits", nullptr, &m_showCredits);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// 2. Credits Window 
	if (m_showCredits)
	{
		ImGui::Begin("Credits", &m_showCredits, ImGuiWindowFlags_AlwaysAutoResize);
		
		if (m_logoLoaded && m_logoTextureSRV)
		{
			// Render the loaded logo image scaled to 150x150
			ImGui::Image((void*)m_logoTextureSRV, ImVec2(150, 150));
		}
		else
		{
			// Fallback placeholder with helpful instruction
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "[Logo Image Missing]");
			ImGui::Text("Place 'logo.png' or 'dlsu_logo.png'");
			ImGui::Text("in the project root directory.");
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		if (ImGui::CollapsingHeader("About", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Zero Systems Engine");
			ImGui::Text("Developed by: Arvin Dacanay");
			ImGui::Spacing();
			ImGui::Text("Acknowledgements:");
			ImGui::BulletText("PardCode Game Engine Tutorial");
			ImGui::BulletText("GDENG03 Course");
		}
		
		ImGui::End();
	}

	// 3. Scene Hierarchy Window
	if (m_showHierarchy)
	{
		ImGui::Begin("Scene Hierarchy", &m_showHierarchy);
		
		auto drawSelectable = [this](const char* label, dx3d::GameObject* obj) {
			bool selected = (m_selectedObject == obj);
			if (ImGui::Selectable(label, selected))
			{
				m_selectedObject = obj;
			}
		};

		drawSelectable("Grid/Plane", m_plane);
		drawSelectable("Utah Teapot (Brick Texture)", m_teapot);
		drawSelectable("Stanford Bunny", m_bunny);
		drawSelectable("Armadillo", m_armadillo);
		drawSelectable("Player / Main Camera", m_player);

		ImGui::End();
	}

	// 4. Inspector Window
	if (m_showInspector)
	{
		ImGui::Begin("Inspector", &m_showInspector);
		if (m_selectedObject)
		{
			// Show name based on object type
			const char* name = "Unknown GameObject";
			if (m_selectedObject == m_plane) name = "Grid/Plane";
			else if (m_selectedObject == m_teapot) name = "Utah Teapot (Brick Texture)";
			else if (m_selectedObject == m_bunny) name = "Stanford Bunny";
			else if (m_selectedObject == m_armadillo) name = "Armadillo";
			else if (m_selectedObject == m_player) name = "Player / Main Camera";

			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "Name: %s", name);
			ImGui::Separator();

			auto& transform = m_selectedObject->getTransform();
			
			// Position
			dx3d::Vec3 pos = transform.getPosition();
			float p[3] = { pos.x, pos.y, pos.z };
			if (ImGui::DragFloat3("Position", p, 0.05f))
			{
				transform.setPosition({ p[0], p[1], p[2] });
			}

			// Rotation
			dx3d::Vec3 rot = transform.getRotation();
			float r[3] = { rot.x, rot.y, rot.z };
			if (ImGui::DragFloat3("Rotation", r, 0.05f))
			{
				transform.setRotation({ r[0], r[1], r[2] });
			}

			// Scale
			dx3d::Vec3 scale = transform.getScale();
			float s[3] = { scale.x, scale.y, scale.z };
			if (ImGui::DragFloat3("Scale", s, 0.05f))
			{
				transform.setScale({ s[0], s[1], s[2] });
			}

			ImGui::Separator();
			
			// Show controls info
			ImGui::TextWrapped("Click & drag values to modify the transform in real time.");
		}
		else
		{
			ImGui::Text("Select a GameObject from the Scene Hierarchy to inspect its properties.");
		}
		ImGui::End();
	}

	// 5. Color Picker Window (Placeholder with Hue Wheel)
	if (m_showColorPicker)
	{
		ImGui::Begin("Color Picker Screen", &m_showColorPicker, ImGuiWindowFlags_AlwaysAutoResize);
		static float color[3] = { 0.5f, 0.8f, 0.6f };
		ImGui::ColorPicker3("Color", color, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::End();
	}
}
