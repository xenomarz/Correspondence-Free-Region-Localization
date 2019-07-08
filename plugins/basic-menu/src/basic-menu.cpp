#include <basic-menu/include/basic-menu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <igl/project.h>
#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_fonts_droid_sans.h>
#include <GLFW/glfw3.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/ViewerPlugin.h>
#include <igl/igl_inline.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;
namespace rds
{
	namespace plugins
	{
		BasicMenu::BasicMenu() :
			igl::opengl::glfw::imgui::ImGuiMenu()
		{

		}

		IGL_INLINE void BasicMenu::draw_viewer_menu()
		{
			// Helper for setting viewport specific mesh options
			auto make_checkbox = [&](const char *label, unsigned int &option, unsigned int &core_id)
			{
				return ImGui::Checkbox(label,
					[&]() { return viewer->core(core_id).is_set(option); },
					[&](bool value) { return viewer->core(core_id).set(option, value); }
				);
			};

			// Mesh
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float w = ImGui::GetContentRegionAvailWidth();
				float p = ImGui::GetStyle().FramePadding.x;
				if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
				{
					viewer->open_dialog_load_mesh();
				}
				ImGui::SameLine(0, p);
				if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
				{
					viewer->open_dialog_save_mesh();
				}
			}

			if (ImGui::Checkbox("Test", &test_bool))
			{
			}

			if (!test_bool)
			{
				/*cout << viewer->core_list.size() << endl;

				unsigned int left_view, right_view;
				int model1_id = viewer->data_list[0].id;
				int model2_id = viewer->data_list[1].id;
				viewer->core().viewport = Eigen::Vector4f(0, 0, 640, 800);
				left_view = viewer->core_list[0].id;
				*/
				//viewer->append_core(Eigen::Vector4f(640, 0, 640, 800));
				unsigned int id = viewer->core_list[0].id;
				int index = viewer->core_index(id);
				
				
				cout << viewer->erase_core(index) << endl;
				test_bool = !test_bool;
				//viewer->erase_core(viewer->core_list[1].id);

				/*cout << viewer->core_list.size() << endl;

				

				viewer->data(model1_id).set_visible(false, left_view);
				viewer->data(model2_id).set_visible(false, right_view);*/
			}

			

			for (auto& core : viewer->core_list)
			{
				ImGui::PushID(core.id);

				std::stringstream ss;
				ss << "Core " << core.id;

				if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::ColorEdit4("Background", core.background_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
				}

				ImGui::PopID();
			}
			

			for (auto& data : viewer->data_list)
			{
				ImGui::PushID(data.id);
				std::stringstream ss;

				if (data_id_to_name.count(data.id) > 0)
				{
					ss << data_id_to_name[data.id];
				}
				else
				{
					ss << "Data " << data.id;
				}

				if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::Checkbox("Face-based", &(viewer->data(data.id).face_based)))
					{
						viewer->data(data.id).dirty = igl::opengl::MeshGL::DIRTY_ALL;
					}

					//make_checkbox("Wireframe", viewer->data(data.id).show_lines);
					//make_checkbox("Fill", viewer->data(data.id).show_faces);

					ImGui::ColorEdit4("Line color", viewer->data(data.id).line_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::DragFloat("Shininess", &(viewer->data(data.id).shininess), 0.05f, 0.0f, 100.0f);
				}
				ImGui::PopID();
			}
		}

		void BasicMenu::set_name_mapping(unsigned int data_id, std::string name)
		{
			data_id_to_name[data_id] = name;
		}
	}
}