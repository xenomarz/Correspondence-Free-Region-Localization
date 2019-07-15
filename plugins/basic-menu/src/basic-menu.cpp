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
				ShowModelIndex = 0;
				param_type = HARMONIC;
				set_name_mapping(0, filename(MODEL1_PATH));
				onMouse_triangle_color = RED_COLOR;
				selected_faces_color = BLUE_COLOR;
				selected_vertices_color = GREEN_COLOR;
				model_color = GOLD_COLOR;
				mouse_mode = NONE;
				view = Horizontal;

				viewer->load_mesh_from_file(std::string(MODEL1_PATH));
				viewer->load_mesh_from_file(std::string(MODEL1_PATH));
												
				viewer->core().viewport = Eigen::Vector4f(0, 0, 640, 800);
				left_view_id = viewer->core(0).id;
				right_view_id = viewer->append_core(Eigen::Vector4f(640, 0, 640, 800));
				viewer->data_list[0].show_texture = true;
				viewer->data_list[1].show_texture = true;
				
				Update_view();
				compute_harmonic_param(1);
			}
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
					//Load new model that has two copies
					std::string fname = igl::file_dialog_open();
					if (fname.length() != 0)
					{
						set_name_mapping(viewer->data_list.size(), filename(fname));
						viewer->load_mesh_from_file(fname.c_str());
						viewer->load_mesh_from_file(fname.c_str());
					}
					///////////////
					

					Update_view();
				}
				ImGui::SameLine(0, p);
				if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
				{
					viewer->open_dialog_save_mesh();
				}
			}


			float col[3] = { onMouse_triangle_color[0] , onMouse_triangle_color[1] ,onMouse_triangle_color[2] };
			ImGui::ColorEdit3("Follow face colors", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			onMouse_triangle_color << col[0], col[1], col[2];

			col[0] = selected_faces_color[0]; col[1] = selected_faces_color[1]; col[2] = selected_faces_color[2];
			ImGui::ColorEdit3("selected faces color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			selected_faces_color << col[0], col[1], col[2];

			col[0] = selected_vertices_color[0]; col[1] = selected_vertices_color[1]; col[2] = selected_vertices_color[2];
			ImGui::ColorEdit3("selected vertices color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			selected_vertices_color << col[0], col[1], col[2];
			
			col[0] = model_color[0]; col[1] = model_color[1]; col[2] = model_color[2];
			ImGui::ColorEdit3("model color", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			model_color << col[0], col[1], col[2];


			View prev_view = view;
			ImGui::Combo("View", (int *)(&view), "Horizontal\0Vertical\0Core_1\0Core_2\0\0");

			MouseMode prev_mouse_mode = mouse_mode;
			ImGui::Combo("Mouse Mode", (int *)(&mouse_mode), "NONE\0FACE_SELECT\0VERTEX_SELECT\0CLEAR\0\0");

			Parametrization prev_param_type = param_type;
			ImGui::Combo("Parametrization type", (int *)(&param_type), "HARMONIC\0LSCM\0ARAP\0\0");

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

			// That's how you get the current width/height of the frame buffer (for example, after the window was resized)
			int frameBufferWidth, frameBufferHeight;
			static int prev_width = 0, prev_height = 0;
			glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);


			//when a change occured on view mode
			if (prev_view != view) {
				post_resize(frameBufferWidth, frameBufferHeight);
			}

			int prev_model = ShowModelIndex;
			ImGui::Combo("Choose model", (int *)(&ShowModelIndex), getModelNames(), IM_ARRAYSIZE(getModelNames()));

			//if a the mesh is changes then update the view
			if (prev_model != ShowModelIndex) {
				Update_view();
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

			follow_and_mark_selected_faces();

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
					colors_per_face.row(i) = model_color;
				}
				//Mark the selected faces
				colors_per_face.row(f) = onMouse_triangle_color;
				for (auto fi : selected_faces) { colors_per_face.row(fi) = selected_faces_color; }

				//Mark the selected vert
				Eigen::MatrixXd P_Left;
				Eigen::MatrixXd P_Right;
				Eigen::MatrixXd C;
				P_Left.resize(selected_vertices.size(), 3);
				P_Right.resize(selected_vertices.size(), 3);
				C.resize(selected_vertices.size(), 3);
				int idx = 0;
				for (auto vi : selected_vertices) {
					P_Left.row(idx) = viewer->data(LeftModelID()).V.row(vi);
					C.row(idx) = selected_vertices_color;
					P_Right.row(idx) = viewer->data(RightModelID()).V.row(vi);
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

		//void draw_menu(igl::opengl::glfw::Viewer* viewer) {
		//	// Viewing options
		//	if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
		//	{
		//		if (ImGui::Button("Center object", ImVec2(-1, 0)))
		//		{
		//			viewer->core().align_camera_center(viewer->data().V, viewer->data().F);
		//		}
		//		if (ImGui::Button("Snap canonical view", ImVec2(-1, 0)))
		//		{
		//			viewer->snap_to_canonical_quaternion();
		//		}

		//		// Zoom
		//		ImGui::PushItemWidth(80 * menu_scaling());
		//		ImGui::DragFloat("Zoom", &(viewer->core().camera_zoom), 0.05f, 0.1f, 20.0f);

		//		// Select rotation type
		//		int rotation_type = static_cast<int>(viewer->core().rotation_type);
		//		static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
		//		static bool orthographic = true;
		//		if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
		//		{
		//			using RT = igl::opengl::ViewerCore::RotationType;
		//			auto new_type = static_cast<RT>(rotation_type);
		//			if (new_type != viewer->core().rotation_type)
		//			{
		//				if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
		//				{
		//					trackball_angle = viewer->core().trackball_angle;
		//					orthographic = viewer->core().orthographic;
		//					viewer->core().trackball_angle = Eigen::Quaternionf::Identity();
		//					viewer->core().orthographic = true;
		//				}
		//				else if (viewer->core().rotation_type == RT::ROTATION_TYPE_NO_ROTATION)
		//				{
		//					viewer->core().trackball_angle = trackball_angle;
		//					viewer->core().orthographic = orthographic;
		//				}
		//				viewer->core().set_rotation_type(new_type);
		//			}
		//		}

		//		// Orthographic view
		//		ImGui::Checkbox("Orthographic view", &(viewer->core().orthographic));
		//		ImGui::PopItemWidth();
		//	}

		//	// Helper for setting viewport specific mesh options
		//	auto make_checkbox = [&](const char *label, unsigned int &option)
		//	{
		//		return ImGui::Checkbox(label,
		//			[&]() { return viewer->core().is_set(option); },
		//			[&](bool value) { return viewer->core().set(option, value); }
		//		);
		//	};

		//	// Draw options
		//	if (ImGui::CollapsingHeader("Draw Options", ImGuiTreeNodeFlags_DefaultOpen))
		//	{
		//		if (ImGui::Checkbox("Face-based", &(viewer->data().face_based)))
		//		{
		//			viewer->data().dirty = MeshGL::DIRTY_ALL;
		//		}
		//		make_checkbox("Show texture", viewer->data().show_texture);
		//		if (ImGui::Checkbox("Invert normals", &(viewer->data().invert_normals)))
		//		{
		//			viewer->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
		//		}
		//		make_checkbox("Show overlay", viewer->data().show_overlay);
		//		make_checkbox("Show overlay depth", viewer->data().show_overlay_depth);
		//		ImGui::ColorEdit4("Background", viewer->core().background_color.data(),
		//			ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		//		ImGui::ColorEdit4("Line color", viewer->data().line_color.data(),
		//			ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		//		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		//		ImGui::DragFloat("Shininess", &(viewer->data().shininess), 0.05f, 0.0f, 100.0f);
		//		ImGui::PopItemWidth();
		//	}

		//	// Overlays
		//	if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
		//	{
		//		make_checkbox("Wireframe", viewer->data().show_lines);
		//		make_checkbox("Fill", viewer->data().show_faces);
		//		ImGui::Checkbox("Show vertex labels", &(viewer->data().show_vertid));
		//		ImGui::Checkbox("Show faces labels", &(viewer->data().show_faceid));
		//	}
		//}
	
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

			// Draw checkerboard texture
			viewer->data(model_index).show_texture = true;
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

			// Draw checkerboard texture
			viewer->data(model_index).show_texture = true;
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

			// Draw checkerboard texture
			viewer->data(model_index).show_texture = true;
			Update_view();
		}

		void BasicMenu::post_resize(int w, int h)
		{
			if (viewer)
			{
				if (view == Horizontal) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(0, 0, w / 2, h);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(w / 2, 0, w - (w / 2), h);
				}
				if (view == Vertical) {
					viewer->core(left_view_id).viewport = Eigen::Vector4f(0, 0, w, h/2);
					viewer->core(right_view_id).viewport = Eigen::Vector4f(0, h/2, w, h - (h / 2));
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