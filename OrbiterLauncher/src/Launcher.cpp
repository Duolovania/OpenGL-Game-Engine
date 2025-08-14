#include "launcher.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <filesystem>

float iconSize = 200;

std::unique_ptr<Texture> iconTextures;
GLuint64 fileIcon, miniFolderIcon;

void Launcher::Init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    StylesConfig();

    rootPath = std::filesystem::current_path().parent_path().string();
    projPath = rootPath + "\\Projects\\";

    iconTextures = std::make_unique<Texture>("res/Application Icons/playbutton.png");

    // Loads the launcher application icons.
    fileIcon = iconTextures->Load("res/Application Icons/fileicon.png", true);
    miniFolderIcon = iconTextures->Load("res/Application Icons/foldericon - mini.png", true);

    launcherSettings = fileManager.LoadLauncherConfig(rootPath + "\\OrbiterLauncher\\config.launchOB");
}

bool Launcher::OnUpdate(float deltaTime, float time)
{
    // Creates ImGui window frames.
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_None);

    ImGui::Begin("Projects");
    ShowProjects();

    if (ImGui::Button("Add Project Folder"))
    {
        const char* file_path = tinyfd_selectFolderDialog(
            "Open a folder",              // Title of the dialog
            (rootPath).c_str()                    // Allow multiple selections (0 for no)
        );

        // Checks if the file exists.
        if (file_path)
        {
            std::cout << projPath << std::endl;
            projPath = rootPath + std::string(file_path).erase(0, rootPath.length()); // Erases the directories leading up to the "Assets" folder.
            if (std::find(launcherSettings.directories.begin(), launcherSettings.directories.end(), file_path) == launcherSettings.directories.end()) launcherSettings.directories.push_back(file_path);
        }
    }

    if (ImGui::Button("Open Project From Disk"))
    {
        const char* filterTypes[4] = { "*.projectOB" };
        const char* file_path = tinyfd_openFileDialog(
            "Open a project",              // Title of the dialog
            (rootPath).c_str(),                         // Default path ("" means current directory)
            1,                          // Number of file filters
            filterTypes,                // File filters (e.g., ["*.txt"])
            "Project",                       // Filter description
            0                           // Allow multiple selections (0 for no)
        );

        // Checks if the file exists.
        if (file_path)
        {
            // Open editor with project.
            std::cout << "Opened file." << std::endl;
            if (std::find(launcherSettings.directories.begin(), launcherSettings.directories.end(), file_path) == launcherSettings.directories.end()) launcherSettings.directories.push_back(file_path);
            editorInstructions.selectedProjPath = file_path;
        }
    }

    if (ImGui::Button("Open Editor test"))
    {
        OpenEditor();
    }
    ImGui::End();

    // Renders ImGui data.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

void Launcher::Close()
{
    fileManager.CreateLauncherConfig(launcherSettings, rootPath + "\\OrbiterLauncher\\config.launchOB");

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void Launcher::StylesConfig()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontDefault = io.Fonts->AddFontFromFileTTF("res/Fonts/open-sans/OpenSans-Semibold.ttf", 18.0f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 3.5f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);

    style.Colors[ImGuiCol_Header] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.055f, 0.055f, 0.055f, 1.0f);

    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.141f, 0.729f, 0.38f, 1.0f);

    style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    style.Colors[ImGuiCol_Tab] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);
    style.Colors[ImGuiCol_TabSelected] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.075f, 0.075f, 0.075f, 1.0f);

    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.177f, 0.479f, 0.177f, 1.0f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.135f, 0.28f, 0.135f, 1.0f);

    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.318f, 0.318f, 0.318f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.697f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.0f);
}

void Launcher::ShowProjects()
{
    if (ImGui::BeginChild("FolderItems"))
    {
        int counter = 0;
        ImVec2 padding = ImVec2(iconSize * 0.125f, iconSize * 0.125f);
        ImVec2 originalPos = ImVec2(ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y + padding.y);

        for (const auto& directory : launcherSettings.directories)
        {
            for (const auto& entry : std::filesystem::directory_iterator(directory))
            {
                if (!entry.is_directory())
                {
                    unsigned int fileThumbnail = fileIcon; // Sets the thumbnail image to the generic file icon by default.
                    int fileNameLength = entry.path().filename().string().find_last_of('.'); // Gets the length of the file name up to the file extension.
                    std::string fileExtension = entry.path().filename().string().substr(fileNameLength); // Gets the file extension.

                    // Checks if the file is an engine project file.
                    if (fileExtension == ".projectOB")
                    {
                        // Excludes file extension from label.
                        std::string buttonText = entry.path().filename().string().erase(fileNameLength, entry.path().filename().string().length());

                        ImVec2 buttonSize = ImVec2(iconSize, iconSize);
                        ImVec2 buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x + padding.x)), originalPos.y); // Calculates the x position based on how many items there are.

                        ImGui::SetWindowFontScale(buttonSize.x / 195.0f);

                        // Sets the text position to the center of the thumbnail (sets the text origin position to the center of the thumbnail and subtracts it by the amount of characters. The subtraction is to ensure that the text is centered regardless of it's length).
                        ImVec2 textPos = ImVec2(buttonPos.x + (buttonSize.x - ImGui::CalcTextSize(buttonText.c_str()).x) / 2, originalPos.y + (buttonSize.y + padding.y));

                        // Checks if the thumbnail will exceed the window size.
                        if ((buttonPos.x + buttonSize.x > ImGui::GetContentRegionMax().x))
                        {
                            counter = 0; // Resets the x position.
                            originalPos = ImVec2(originalPos.x, ImGui::GetCursorPos().y + ((buttonSize.y / 4) + padding.y)); // Calculates the y position based on the button size and padding amount.

                            buttonPos = ImVec2(originalPos.x + (counter * (buttonSize.x + padding.x)), originalPos.y); // Recalculates the button position with the x position being reset.
                            textPos = ImVec2(buttonPos.x + (buttonSize.x - ImGui::CalcTextSize(buttonText.c_str()).x) / 2, originalPos.y + (buttonSize.y + padding.y)); // Recalculates the text position with the x position being reset.
                        }

                        ImGui::PushID(counter);
                        ImGui::SetCursorPos(buttonPos);

                        // Creates the file buttons.
                        if (ImGui::ImageButton((void*)fileThumbnail, buttonSize, ImVec2(0, 1), ImVec2(1, 0)))
                        {
                            std::cout << "do stuff" << std::endl;
                        };


                        ImGui::SetCursorPos(textPos);

                        ImGui::Text(buttonText.c_str()); // Creates the file label.
                        ImGui::SameLine();
                        ImGui::PopID();

                        counter++;
                    }
                }
            }
        }

        

        ImGui::EndChild();
    }
}

void Launcher::OpenEditor()
{
    std::cout << rootPath + "/OrbiterEditor/launchinstructions.instructOB" << std::endl;
    fileManager.CreateEditorInstructions(editorInstructions, rootPath + "/OrbiterEditor/launchinstructions.instructOB");
}
