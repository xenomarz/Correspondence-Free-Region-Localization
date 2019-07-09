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
#include <igl/unproject_in_mesh.h>
#include <igl/Hit.h>
#include <igl/rotate_by_quat.h>
#include <memory>

using namespace std;
namespace rds
{
	namespace plugins
	{
		BasicMenu::BasicMenu() :
			igl::opengl::glfw::imgui::ImGuiMenu()
		{
			show_models[0] = 1;
			show_models[1] = 0;
			test_bool = 0;
			set_name_mapping(0,"wolf");
			set_name_mapping(1,"cow");
			set_name_mapping(2,"cube");
			
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
					for (auto& core : viewer->core_list)
					{
						for (auto& data : viewer->data_list)
						{
							viewer->data(data.id).set_visible(false, core.id);
						}
						viewer->data(show_models[core.id - 1]).set_visible(true, core.id);
						Eigen::MatrixXd V = viewer->data(show_models[core.id - 1]).V;
						Eigen::MatrixXi F = viewer->data(show_models[core.id - 1]).F;
						core.align_camera_center(V, F);
					}

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

			

			
			Orientation prev_view = view;
			ImGui::Combo("View", (int *)(&view), "Two views\0Left view\0Right view\0\0");
			

			// That's how you get the current width/height of the frame buffer (for example, after the window was resized)
			int frameBufferWidth, frameBufferHeight;
			static int prev_width = 0, prev_height = 0;
			glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);
			

			//when a change occured on view mode
			if (prev_view != view) {
				if (view == Two_views) {
					viewer->core_list[0].viewport = Eigen::Vector4f(0, 0, frameBufferWidth / 2, frameBufferHeight);
					viewer->core_list[1].viewport = Eigen::Vector4f(frameBufferWidth / 2, 0, frameBufferWidth - (frameBufferWidth / 2), frameBufferHeight);
				}
				if (view == Left_view) {
					viewer->core_list[0].viewport = Eigen::Vector4f(0, 0, frameBufferWidth, frameBufferHeight);
					viewer->core_list[1].viewport = Eigen::Vector4f(frameBufferWidth + 1, frameBufferHeight + 1, frameBufferWidth + 2, frameBufferHeight + 2);
				}
				if (view == Right_view) {
					viewer->core_list[0].viewport = Eigen::Vector4f(frameBufferWidth + 1, frameBufferHeight + 1, frameBufferWidth + 2, frameBufferHeight + 2);
					viewer->core_list[1].viewport = Eigen::Vector4f(0, 0, frameBufferWidth, frameBufferHeight);
				}
			}

			
			

			for (auto& core : viewer->core_list)
			{
				ImGui::PushID(core.id);

				std::stringstream ss;
				ss << "Core " << core.id;

				if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					char* items = getModelNames();
					ImGui::ColorEdit4("Background", core.background_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					
					int prev_model = show_models[core.id - 1];
					ImGui::Combo("Choose model", (int *)(&show_models[core.id-1]), items, IM_ARRAYSIZE(items));
					if (prev_model != show_models[core.id - 1]) {
						for (auto& data : viewer->data_list)
						{
							viewer->data(data.id).set_visible(false, core.id);
						}
						viewer->data(show_models[core.id - 1]).set_visible(true, core.id);
						Eigen::MatrixXd V = viewer->data(show_models[core.id - 1]).V;
						Eigen::MatrixXi F = viewer->data(show_models[core.id - 1]).F;
						core.align_camera_center(V, F);
					}
				}

				if (test_bool) {
					////////////////////////
					int down_mouse_x = viewer->current_mouse_x;
					int down_mouse_y = viewer->current_mouse_y;

					Eigen::MatrixXd V = viewer->data(show_models[core.id - 1]).V;
					Eigen::MatrixXi F = viewer->data(show_models[core.id - 1]).F;



					//////////////////////////////////////////////////////////////////
					//int f = pick_face(viewer, down_mouse_x, down_mouse_y, V, F);

					double x = down_mouse_x;
					double y = core.viewport(3) - down_mouse_y;

					Eigen::RowVector3d pt;

					Eigen::Matrix4f modelview = core.view;
					int vi = -1;

					std::vector<igl::Hit> hits;

					igl::unproject_in_mesh(Eigen::Vector2f(x, y), core.view,
						core.proj, core.viewport, V, F, pt, hits);

					int f = -1;
					if (hits.size() > 0) {
						f = hits[0].id;
					}
					/////////////////////////////////////////////////////////////////

					if (f != -1)
					{
						//selected_faces.insert(f);
						//selected_v = -1;
						Eigen::MatrixXd colors_per_face;
						colors_per_face.resize(F.rows(), 3);
						for (int i = 0; i < colors_per_face.rows(); i++)
						{
							colors_per_face.row(i) << 1, 215.0f / 255.0f, 0;
						}
						colors_per_face.row(f) << 1, 0, 0;
						viewer->data(show_models[core.id - 1]).set_colors(colors_per_face);
					}
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

		


		char* BasicMenu::getModelNames()
		{
			std::string cStr = "";
			for (auto& data : viewer->data_list)
			{
				std::stringstream ss;
				if (data_id_to_name.count(data.id) > 0)
				{
					ss << data_id_to_name[data.id];
				}
				else
				{
					ss << "Model " << data.id;
				}

				//char curr_name = data.id + '0';
				
				cStr += ss.str().c_str();
				cStr += " ";
				cStr += '\0';
			}
			cStr += '\0';

			int listLength = cStr.length();
			char* comboList = new char[listLength];
			if (listLength == 1) { comboList[0] = cStr.at(0); }
			for (size_t i = 0; i < listLength; i++) {
				comboList[i] = cStr.at(i);
			}
			return comboList;
		}

		int pick_face(igl::opengl::glfw::Viewer* viewer, int mouse_x, int mouse_y, const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {
			// Cast a ray in the view direction starting from the mouse position
			double x = mouse_x;
			double y = viewer->core().viewport(3) - mouse_y;

			Eigen::RowVector3d pt;

			Eigen::Matrix4f modelview = viewer->core().view;
			int vi = -1;

			std::vector<igl::Hit> hits;

			igl::unproject_in_mesh(Eigen::Vector2f(x, y), viewer->core().view,
				viewer->core().proj, viewer->core().viewport, V, F, pt, hits);

			int fi = -1;
			if (hits.size() > 0) {
				fi = hits[0].id;
			}
			return fi;
		}
		void draw_menu(igl::opengl::glfw::Viewer* viewer) {
			// Viewing options
			if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Button("Center object", ImVec2(-1, 0)))
				{
					viewer->core().align_camera_center(viewer->data().V, viewer->data().F);
				}
				if (ImGui::Button("Snap canonical view", ImVec2(-1, 0)))
				{
					viewer->snap_to_canonical_quaternion();
				}

				// Zoom
				//ImGui::PushItemWidth(80 * menu_scaling());
				ImGui::DragFloat("Zoom", &(viewer->core().camera_zoom), 0.05f, 0.1f, 20.0f);

				// Select rotation type
				int rotation_type = static_cast<int>(viewer->core().rotation_type);
				static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
				static bool orthographic = true;
				if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
				{
					using RT = igl::opengl::ViewerCore::RotationType;
					auto new_type = static_cast<RT>(rotation_type);
					if (new_type != viewer->core().rotation_type)
					{
						if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
						{
							trackball_angle = viewer->core().trackball_angle;
							orthographic = viewer->core().orthographic;
							viewer->core().trackball_angle = Eigen::Quaternionf::Identity();
							viewer->core().orthographic = true;
						}
						else if (viewer->core().rotation_type == RT::ROTATION_TYPE_NO_ROTATION)
						{
							viewer->core().trackball_angle = trackball_angle;
							viewer->core().orthographic = orthographic;
						}
						viewer->core().set_rotation_type(new_type);
					}
				}

				// Orthographic view
				ImGui::Checkbox("Orthographic view", &(viewer->core().orthographic));
				ImGui::PopItemWidth();
			}

			// Helper for setting viewport specific mesh options
			auto make_checkbox = [&](const char *label, unsigned int &option)
			{
				return ImGui::Checkbox(label,
					[&]() { return viewer->core().is_set(option); },
					[&](bool value) { return viewer->core().set(option, value); }
				);
			};

			// Draw options
			if (ImGui::CollapsingHeader("Draw Options", ImGuiTreeNodeFlags_DefaultOpen))
			{
				/*if (ImGui::Checkbox("Face-based", &(viewer->data().face_based)))
				{
					viewer->data().dirty = MeshGL::DIRTY_ALL;
				}
				make_checkbox("Show texture", viewer->data().show_texture);
				if (ImGui::Checkbox("Invert normals", &(viewer->data().invert_normals)))
				{
					viewer->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
				}
				make_checkbox("Show overlay", viewer->data().show_overlay);
				make_checkbox("Show overlay depth", viewer->data().show_overlay_depth);
				ImGui::ColorEdit4("Background", viewer->core().background_color.data(),
					ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
				ImGui::ColorEdit4("Line color", viewer->data().line_color.data(),
					ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
				ImGui::DragFloat("Shininess", &(viewer->data().shininess), 0.05f, 0.0f, 100.0f);
				ImGui::PopItemWidth();*/
			}

			// Overlays
			if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
			{
				make_checkbox("Wireframe", viewer->data().show_lines);
				make_checkbox("Fill", viewer->data().show_faces);
				ImGui::Checkbox("Show vertex labels", &(viewer->data().show_vertid));
				ImGui::Checkbox("Show faces labels", &(viewer->data().show_faceid));
			}
		}
	}
}