#include <basic-menu/include/basic-menu.h>


namespace rds
{
	namespace plugins
	{
		BasicMenu::BasicMenu() :
			igl::opengl::glfw::imgui::ImGuiMenu()
		{
				
		}

		IGL_INLINE void BasicMenu::init(igl::opengl::glfw::Viewer *_viewer)
		{
			ImGuiMenu::init(_viewer);

			if (_viewer)
			{
				//Basic (necessary) parameteres
				ShowModelIndex = 0;
				core_percentage_size = 0.5;
				param_type = HARMONIC;
				set_name_mapping(0, filename(MODEL1_PATH));
				onMouse_triangle_color = RED_COLOR;
				selected_faces_color = BLUE_COLOR;
				selected_vertices_color = GREEN_COLOR;
				model_color = GOLD_COLOR;
				mouse_mode = NONE;
				view = Horizontal;

				//Solver Parameters
				SolverMode = false;

				//Parametrization Parameters
				Position_Weight = Seamless_Weight = Integer_Spacing = Integer_Weight = Delta = Lambda = 0.5;
					
				//Load model
				viewer->load_mesh_from_file(std::string(MODEL1_PATH));
				viewer->load_mesh_from_file(std::string(MODEL1_PATH));	

				//Load two views
				viewer->core().viewport = Eigen::Vector4f(0, 0, 640, 800);
				left_view_id = viewer->core(0).id;
				right_view_id = viewer->append_core(Eigen::Vector4f(640, 0, 640, 800));

				//Update scene
				Update_view();
				compute_harmonic_param(1);
			}
		}

		IGL_INLINE void BasicMenu::draw_viewer_menu()
		{
			float w = ImGui::GetContentRegionAvailWidth();
			float p = ImGui::GetStyle().FramePadding.x;
			if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
			{
				//Load new model that has two copies
				std::string fname = igl::file_dialog_open();
				if (fname.length() != 0)
				{
					set_name_mapping(viewer->data_list.size(), filename(fname));
					viewer->load_mesh_from_file(fname.c_str());
					viewer->load_mesh_from_file(fname.c_str());
				}
				Update_view();
			}
			ImGui::SameLine(0, p);
			if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
			{
				viewer->open_dialog_save_mesh();
			}
			
			ImGui::ColorEdit3("Follow face colors", onMouse_triangle_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			ImGui::ColorEdit3("selected faces color", selected_faces_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			ImGui::ColorEdit3("selected vertices color", selected_vertices_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			ImGui::ColorEdit3("model color", model_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);

			View prev_view = view;
			ImGui::Combo("View", (int *)(&view), "Horizontal\0Vertical\0Core_1\0Core_2\0\0");
			if ((view == Horizontal) || (view == Vertical)) {
				float prev_size = core_percentage_size;
				ImGui::SliderFloat("Core Size", &core_percentage_size, 0, 1, to_string(core_percentage_size).c_str(), 1);
				//when a change occured on core percentage size
				if (prev_size != core_percentage_size) {
					// That's how you get the current width/height of the frame buffer (for example, after the window was resized)
					int frameBufferWidth, frameBufferHeight;
					glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);
					post_resize(frameBufferWidth, frameBufferHeight);
				}
			}

			MouseMode prev_mouse_mode = mouse_mode;
			ImGui::Combo("Mouse Mode", (int *)(&mouse_mode), "NONE\0FACE_SELECT\0VERTEX_SELECT\0CLEAR\0\0");

			Parametrization prev_param_type = param_type;
			ImGui::Combo("Parametrization type", (int *)(&param_type), "HARMONIC\0LSCM\0ARAP\0\0");

			int prev_model = ShowModelIndex;
			ImGui::Combo("Choose model", (int *)(&ShowModelIndex), getModelNames(), IM_ARRAYSIZE(getModelNames()));

			//when a change occured on mouse mode
			if (prev_mouse_mode != mouse_mode) {
				if (mouse_mode == CLEAR) {
					selected_faces.clear();
					selected_vertices.clear();
				}
			}
			
			//when a change occured on parametrization type
			if (prev_param_type != param_type) {
				if (param_type == HARMONIC) {
					compute_harmonic_param(RightModelID());
				}
				else if (param_type == LSCM) {
					compute_lscm_param(RightModelID());
				}
				else {
					compute_ARAP_param(RightModelID());
				}
			}

			//when a change occured on view mode
			if (prev_view != view) {
				// That's how you get the current width/height of the frame buffer (for example, after the window was resized)
				int frameBufferWidth, frameBufferHeight;
				glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);
				post_resize(frameBufferWidth, frameBufferHeight);
			}

			//if a the mesh is changes then update the view
			if (prev_model != ShowModelIndex) {
				Update_view();
			}

			Draw_menu_for_Parametrization();
			Draw_menu_for_Solver();
			Draw_menu_for_cores();
			Draw_menu_for_models();
			
			follow_and_mark_selected_faces();

			
		}


		void BasicMenu::Draw_menu_for_Solver() {
			if (ImGui::CollapsingHeader("Solver", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (SolverMode)
					ImGui::Checkbox("On", &SolverMode);
				else
					ImGui::Checkbox("Off", &SolverMode);
			}
		}


		void BasicMenu::Draw_menu_for_Parametrization() {
			if (ImGui::CollapsingHeader("Energy Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
				float prev_Lambda = Lambda;
				float prev_Delta = Delta;
				float prev_Integer_Weight = Integer_Weight;
				float prev_Integer_Spacing = Integer_Spacing;
				float prev_Seamless_Weight = Seamless_Weight;
				float prev_Position_Weight = Position_Weight;


				ImGui::SliderFloat("Lambda", &Lambda, 0, 1, to_string(Lambda).c_str(), 1);
				ImGui::SliderFloat("Delta", &Delta, 0, 1, to_string(Delta).c_str(), 1);
				ImGui::SliderFloat("Integer Weight", &Integer_Weight, 0, 1, to_string(Integer_Weight).c_str(), 1);
				ImGui::SliderFloat("Integer Spacing", &Integer_Spacing, 0, 1, to_string(Integer_Spacing).c_str(), 1);
				ImGui::SliderFloat("Seamless Weight", &Seamless_Weight, 0, 1, to_string(Seamless_Weight).c_str(), 1);
				ImGui::SliderFloat("Position Weight", &Position_Weight, 0, 1, to_string(Position_Weight).c_str(), 1);


				//when a change occured on Lambda
				if (prev_Lambda != Lambda) {

				}
				//when a change occured on Delta
				if (prev_Delta != Delta) {

				}
				//when a change occured on Integer_Weight
				if (prev_Integer_Weight != Integer_Weight) {

				}
				//when a change occured on Integer_Spacing
				if (prev_Integer_Spacing != Integer_Spacing) {

				}
				//when a change occured on Seamless_Weight
				if (prev_Seamless_Weight != Seamless_Weight) {

				}
				//when a change occured on Position_Weight
				if (prev_Position_Weight != Position_Weight) {

				}

			}
		}

		void BasicMenu::Draw_menu_for_cores() {
			for (auto& core : viewer->core_list)
			{
				ImGui::PushID(core.id);
				std::stringstream ss;
				ss << "Core " << core.id;
				if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					int data_id = RightModelID();
					if (core.id == 1) {
						data_id = LeftModelID();
					}

					if (ImGui::Button("Center object", ImVec2(-1, 0)))
					{
						core.align_camera_center(viewer->data(data_id).V, viewer->data(data_id).F);
					}
					if (ImGui::Button("Snap canonical view", ImVec2(-1, 0)))
					{
						viewer->snap_to_canonical_quaternion();
					}

					// Zoom
					ImGui::PushItemWidth(80 * menu_scaling());
					ImGui::DragFloat("Zoom", &(core.camera_zoom), 0.05f, 0.1f, 20.0f);

					// Select rotation type
					int rotation_type = static_cast<int>(core.rotation_type);
					static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
					static bool orthographic = true;
					if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
					{
						using RT = igl::opengl::ViewerCore::RotationType;
						auto new_type = static_cast<RT>(rotation_type);
						if (new_type != core.rotation_type)
						{
							if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
							{
								trackball_angle = core.trackball_angle;
								orthographic = core.orthographic;
								core.trackball_angle = Eigen::Quaternionf::Identity();
								core.orthographic = true;
							}
							else if (core.rotation_type == RT::ROTATION_TYPE_NO_ROTATION)
							{
								core.trackball_angle = trackball_angle;
								core.orthographic = orthographic;
							}
							core.set_rotation_type(new_type);
						}
					}

					// Orthographic view
					ImGui::Checkbox("Orthographic view", &(core.orthographic));
					ImGui::PopItemWidth();
					ImGui::ColorEdit4("Background", core.background_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
				}
				Update_view();
				ImGui::PopID();
			}
		}

		void BasicMenu::Draw_menu_for_models() {
			for (auto& data : viewer->data_list)
			{
				// Helper for setting viewport specific mesh options
				auto make_checkbox = [&](const char *label, unsigned int &option, int data_id)
				{
					int core_id = right_view_id;
					if (data_id % 2 == 0) {
						core_id = left_view_id;
					}

					return ImGui::Checkbox(label,
						[&]() { return viewer->core(core_id).is_set(option); },
						[&](bool value) { return viewer->core(core_id).set(option, value); }
					);
				};

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
					if (ImGui::Checkbox("Face-based", &(data.face_based)))
					{
						data.dirty = igl::opengl::MeshGL::DIRTY_ALL;
					}

					make_checkbox("Show texture", data.show_texture, data.id);
					if (ImGui::Checkbox("Invert normals", &(data.invert_normals)))
					{
						data.dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
					}
					make_checkbox("Show overlay", data.show_overlay, data.id);
					make_checkbox("Show overlay depth", data.show_overlay_depth, data.id);
					ImGui::ColorEdit4("Line color", data.line_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
					ImGui::DragFloat("Shininess", &(data.shininess), 0.05f, 0.0f, 100.0f);
					ImGui::PopItemWidth();

					make_checkbox("Wireframe", data.show_lines, data.id);
					make_checkbox("Fill", data.show_faces, data.id);
					ImGui::Checkbox("Show vertex labels", &(data.show_vertid));
					ImGui::Checkbox("Show faces labels", &(data.show_faceid));
				}
				Update_view();
				ImGui::PopID();
			}
		}

		void BasicMenu::Update_view() {
			viewer->data().copy_options(viewer->core_list[0], viewer->core_list[1]);
			for (auto& core : viewer->core_list)
			{
				for (auto& data : viewer->data_list)
				{
					viewer->data(data.id).set_visible(false, core.id);
				}
			}
			
			viewer->data(LeftModelID()).set_visible(true, left_view_id);
			viewer->core(left_view_id).align_camera_center(viewer->data(LeftModelID()).V, viewer->data(LeftModelID()).F);

			viewer->data(RightModelID()).set_visible(true, right_view_id);
			viewer->core(right_view_id).align_camera_center(viewer->data(RightModelID()).V, viewer->data(RightModelID()).F);
		}

		void BasicMenu::follow_and_mark_selected_faces() {
			//check if there faces which is selected on the left screen
			int f = pick_face(viewer->data(LeftModelID()).V, viewer->data(LeftModelID()).F, Core_1);
			if (f == -1) {
				//check if there faces which is selected on the right screen
				f = pick_face(viewer->data(RightModelID()).V, viewer->data(RightModelID()).F, Core_2);
			}

			if (f != -1)
			{
				colors_per_face.resize(viewer->data(LeftModelID()).F.rows(), 3);
				for (int i = 0; i < colors_per_face.rows(); i++)
				{
					colors_per_face.row(i) << double(model_color[0]), double(model_color[1]), double(model_color[2]);
				}
				//Mark the selected faces
				colors_per_face.row(f) << double(onMouse_triangle_color[0]) , double(onMouse_triangle_color[1]) , double(onMouse_triangle_color[2]);
				for (auto fi : selected_faces) { colors_per_face.row(fi) << double(selected_faces_color[0]), double(selected_faces_color[1]), double(selected_faces_color[2]); }

				//Mark the selected vertices
				Eigen::MatrixXd P_Left;
				Eigen::MatrixXd P_Right;
				Eigen::MatrixXd C;
				P_Left.resize(selected_vertices.size(), 3);
				P_Right.resize(selected_vertices.size(), 3);
				C.resize(selected_vertices.size(), 3);
				int idx = 0;
				for (auto vi : selected_vertices) {
					P_Left.row(idx) = viewer->data(LeftModelID()).V.row(vi);
					C.row(idx) << double(selected_vertices_color[0]), double(selected_vertices_color[1]), double(selected_vertices_color[2]);
					P_Right.row(idx) = viewer->data(RightModelID()).V.row(vi);
					idx++;
				}
				viewer->data(LeftModelID()).set_points(P_Left, C);
				viewer->data(RightModelID()).set_points(P_Right, C);

				//Update the model's faces colors in the two screens
				viewer->data(LeftModelID()).set_colors(colors_per_face);
				viewer->data(RightModelID()).set_colors(colors_per_face);
			}
		}

		bool BasicMenu::mouse_down(int button, int modifier) {
			if (mouse_mode == FACE_SELECT)
			{
				//check if there faces which is selected on the left screen
				int f = pick_face(viewer->data(LeftModelID()).V, viewer->data(LeftModelID()).F, Core_1);
				if (f == -1) {
					//check if there faces which is selected on the right screen
					f = pick_face(viewer->data(RightModelID()).V, viewer->data(RightModelID()).F, Core_2);
				}

				if (f != -1)
				{
					selected_faces.insert(f);
				}

			}
			else if (mouse_mode == VERTEX_SELECT)
			{
				MatrixXd vertices;
				//check if there faces which is selected on the left screen
				int v = pick_vertex(viewer->data(LeftModelID()).V, viewer->data(LeftModelID()).F, Core_1);
				vertices = viewer->data(LeftModelID()).V;
				if (v == -1) {
					//check if there faces which is selected on the right screen
					v = pick_vertex(viewer->data(RightModelID()).V, viewer->data(RightModelID()).F, Core_2);
					vertices = viewer->data(RightModelID()).V;
				}

				if (v != -1)
				{
					selected_vertices.insert(v);
				}
			}

			return false;
		}
	
		void BasicMenu::set_name_mapping(unsigned int data_id, string name)
		{
			data_id_to_name[data_id] = name;
			data_id_to_name[data_id+1] = name + " (Param.)";
		}

		int BasicMenu::LeftModelID() {
			return 2 * ShowModelIndex;
		}

		int BasicMenu::RightModelID() {
			return (2 * ShowModelIndex) + 1;
		}

		char* BasicMenu::getModelNames()
		{
			std::string cStr = "";
			for (auto& data : viewer->data_list)
			{
				std::stringstream ss;
				if (data.id % 2 == 0) {
					if (data_id_to_name.count(data.id) > 0)
					{
						ss << data_id_to_name[data.id];
					}
					else
					{
						ss << "Model " << data.id;
					}
					cStr += ss.str().c_str();
					cStr += " ";
					cStr += '\0';
				}
				
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

		string BasicMenu::filename(const string& str)
		{
			size_t head,tail;
			head = str.find_last_of("/\\");
			tail = str.find_last_of("/.");
			return (str.substr((head + 1),(tail-head-1)));
		}

		int BasicMenu::pick_face(Eigen::MatrixXd& V,Eigen::MatrixXi& F, View LR) {
			// Cast a ray in the view direction starting from the mouse position
			int core_index;
			if (LR == Core_2) {
				core_index = right_view_id;
			}
			else if (LR == Core_1) {
				core_index = left_view_id;
			}
			double x = viewer->current_mouse_x;
			double y = viewer->core(core_index).viewport(3) - viewer->current_mouse_y;
			if (view == Vertical) {
				y = (viewer->core(left_view_id).viewport(3) / core_percentage_size) - viewer->current_mouse_y;
			}


			Eigen::RowVector3d pt;

			Eigen::Matrix4f modelview = viewer->core(core_index).view;
			int vi = -1;

			std::vector<igl::Hit> hits;

			igl::unproject_in_mesh(Eigen::Vector2f(x, y), viewer->core(core_index).view,
				viewer->core(core_index).proj, viewer->core(core_index).viewport, V, F, pt, hits);

			int fi = -1;
			if (hits.size() > 0) {
				fi = hits[0].id;
			}
			return fi;
		}

		int BasicMenu::pick_vertex(Eigen::MatrixXd& V, Eigen::MatrixXi& F,View LR) {
			// Cast a ray in the view direction starting from the mouse position
			int core_index;
			if (LR == Core_2) {
				core_index = right_view_id;
			}
			else if (LR == Core_1) {
				core_index = left_view_id;
			}

			double x = viewer->current_mouse_x;
			double y = viewer->core(core_index).viewport(3) - viewer->current_mouse_y;
			if (view == Vertical) {
				y = (viewer->core(left_view_id).viewport(3) / core_percentage_size) - viewer->current_mouse_y;
			}

			Eigen::RowVector3d pt;

			Eigen::Matrix4f modelview = viewer->core(core_index).view;
			int vi = -1;

			std::vector<igl::Hit> hits;
			
			igl::unproject_in_mesh(Eigen::Vector2f(x, y), viewer->core(core_index).view,
				viewer->core(core_index).proj, viewer->core(core_index).viewport, V, F, pt, hits);

			if (hits.size() > 0) {
				int fi = hits[0].id;
				Eigen::RowVector3d bc;
				bc << 1.0 - hits[0].u - hits[0].v, hits[0].u, hits[0].v;
				bc.maxCoeff(&vi);
				vi = F(fi, vi);
			}
			return vi;
		}
	
		void BasicMenu::compute_ARAP_param(int model_index) {
			// Compute the initial solution for ARAP (harmonic parametrization)
			Eigen::VectorXi bnd;
			Eigen::MatrixXd V_uv, initial_guess;

			igl::boundary_loop(viewer->data(model_index).F, bnd);
			Eigen::MatrixXd bnd_uv;
			igl::map_vertices_to_circle(viewer->data(model_index).V, bnd, bnd_uv);

			igl::harmonic(viewer->data(model_index).V, viewer->data(model_index).F, bnd, bnd_uv, 1, initial_guess);

			// Add dynamic regularization to avoid to specify boundary conditions
			igl::ARAPData arap_data;
			arap_data.with_dynamics = true;
			Eigen::VectorXi b = Eigen::VectorXi::Zero(0);
			Eigen::MatrixXd bc = Eigen::MatrixXd::Zero(0, 0);

			// Initialize ARAP
			arap_data.max_iter = 100;
			// 2 means that we're going to *solve* in 2d
			arap_precomputation(viewer->data(model_index).V, viewer->data(model_index).F, 2, b, arap_data);


			// Solve arap using the harmonic map as initial guess
			V_uv = initial_guess;

			arap_solve(bc, arap_data, V_uv);


			// Scale UV to make the texture more clear
			V_uv *= 20;


			// Plot the mesh
			viewer->data(model_index).set_mesh(viewer->data(model_index).V, viewer->data(model_index).F);
			viewer->data(model_index).set_uv(V_uv);

			viewer->data(model_index).set_mesh(viewer->data(model_index).V_uv, viewer->data(model_index).F);

			viewer->data(model_index).compute_normals();

			viewer->core(right_view_id).align_camera_center(viewer->data(model_index).V_uv, viewer->data(model_index).F);
			Update_view();
		}

		void BasicMenu::compute_harmonic_param(int model_index) {
			// Find the open boundary
			Eigen::VectorXi bnd;
			Eigen::MatrixXd V_uv;
			igl::boundary_loop(viewer->data(model_index).F, bnd);

			// Map the boundary to a circle, preserving edge proportions
			Eigen::MatrixXd bnd_uv;
			igl::map_vertices_to_circle(viewer->data(model_index).V, bnd, bnd_uv);

			// Harmonic parametrization for the internal vertices
			igl::harmonic(viewer->data(model_index).V, viewer->data(model_index).F, bnd, bnd_uv, 1, V_uv);

			// Scale UV to make the texture more clear
			V_uv *= 5;

			// Plot the mesh
			viewer->data(model_index).set_mesh(viewer->data(model_index).V, viewer->data(model_index).F);
			viewer->data(model_index).set_uv(V_uv);

			viewer->data(model_index).set_mesh(viewer->data(model_index).V_uv, viewer->data(model_index).F);

			viewer->data(model_index).compute_normals();
			viewer->core(right_view_id).align_camera_center(viewer->data(model_index).V_uv, viewer->data(model_index).F);
			Update_view();
		}

		void BasicMenu::compute_lscm_param(int model_index)
		{
			// Fix two points on the boundary
			VectorXi bnd, b(2, 1);
			Eigen::MatrixXd V_uv;
			igl::boundary_loop(viewer->data(model_index).F, bnd);
			b(0) = bnd(0);
			b(1) = bnd(round(bnd.size() / 2));
			MatrixXd bc(2, 2);
			bc << 0, 0, 1, 0;

			// LSCM parametrization
			igl::lscm(viewer->data(model_index).V, viewer->data(model_index).F, b, bc, V_uv);

			// Scale UV to make the texture more clear
			V_uv *= 5;

			// Plot the mesh
			viewer->data(model_index).set_mesh(viewer->data(model_index).V, viewer->data(model_index).F);
			viewer->data(model_index).set_uv(V_uv);

			viewer->data(model_index).set_mesh(viewer->data(model_index).V_uv, viewer->data(model_index).F);

			viewer->data(model_index).compute_normals();
			viewer->core(right_view_id).align_camera_center(viewer->data(model_index).V_uv, viewer->data(model_index).F);
			Update_view();
		}

		void BasicMenu::post_resize(int w, int h)
		{
			if (viewer)
			{
				if (view == Horizontal) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(0, 0, w * core_percentage_size, h);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(w * core_percentage_size, 0, w - (w * core_percentage_size), h);
				}
				if (view == Vertical) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(0, 0, w, h * core_percentage_size);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(0, h* core_percentage_size, w, h - (h * core_percentage_size));
				}
				if (view == Core_1) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(0, 0, w, h);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
				}
				if (view == Core_2) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(w + 1, h + 1, w + 2, h + 2);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(0, 0, w, h);
				}
			}
		}
	}
}