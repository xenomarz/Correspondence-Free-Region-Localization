#include <basic-menu/include/basic-menu.h>

BasicMenu::BasicMenu() :
	opengl::glfw::imgui::ImGuiMenu(){}

IGL_INLINE void BasicMenu::init(opengl::glfw::Viewer *_viewer)
{
	ImGuiMenu::init(_viewer);

	if (_viewer)
	{
		//Basic (necessary) parameteres
		texture_size = 0.5;
				
		core_percentage_size = 0.5;
		param_type = None;
		Max_Distortion = 5;
		Highlighted_face_color = RED_COLOR;
		Fixed_face_color = BLUE_COLOR;
		Dragged_face_color = GREEN_COLOR;
		Vertex_Energy_color = RED_COLOR;
		Dragged_vertex_color = GREEN_COLOR;
		Fixed_vertex_color = BLUE_COLOR;
		model_color = GREY_COLOR;
		text_color = WHITE_COLOR;
		mouse_mode = NONE;
		view = Horizontal;
		IsTranslate = false;
		Highlighted_face = true;
		show_text = true;
		distortion_type = TOTAL_DISTORTION;
		solverInitialized = false;
		down_mouse_x = down_mouse_y = -1;

		//Solver Parameters
		solver_on = false;
		
		//Load two views
		viewer->core().viewport = Vector4f(0, 0, 640, 800);
		input_view_id = viewer->core(0).id;
		output_view_id = viewer->append_core(Vector4f(640, 0, 640, 800));
		viewer->core(output_view_id).background_color += Vector4f(0.1,0,0,0);

		//set rotarion type to 2D mode
		viewer->core(output_view_id).trackball_angle = Quaternionf::Identity();
		viewer->core(output_view_id).orthographic = true;
		viewer->core(output_view_id).set_rotation_type(ViewerCore::RotationType(2));
		
		//Update scene
		Update_view();
		viewer->core(input_view_id).align_camera_center(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F);
		viewer->core(output_view_id).align_camera_center(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F);

		viewer->core(input_view_id).is_animating = true;
		viewer->core(output_view_id).is_animating = true;

		// Initialize solver thread
		solver = make_unique<Newton>();
		totalObjective = make_shared<TotalObjective>();	
		constraintsPositional = make_shared<PenaltyPositionalConstraints>();

		//maximize window
		glfwMaximizeWindow(viewer->window);
	}
}

IGL_INLINE void BasicMenu::draw_viewer_menu()
{
	float w = ImGui::GetContentRegionAvailWidth();
	float p = ImGui::GetStyle().FramePadding.x;
	if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
	{
		//Load new model that has two copies
		string fname = file_dialog_open();
		if (fname.length() != 0)
		{
			stop_solver_thread();
			
			set_name_mapping(0, filename(fname));
			viewer->load_mesh_from_file(fname.c_str());
			viewer->load_mesh_from_file(fname.c_str());
			
			initializeSolver();
			Update_view();
			viewer->core(input_view_id).align_camera_center(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F);
			viewer->core(output_view_id).align_camera_center(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F);
		}
	}
	ImGui::SameLine(0, p);
	if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
	{
		viewer->open_dialog_save_mesh();
	}
			
	ImGui::Checkbox("Highlight faces", &Highlighted_face);
	ImGui::Checkbox("Show text", &show_text);

	if ((view == Horizontal) || (view == Vertical)) {
		if(ImGui::SliderFloat("Core Size", &core_percentage_size, 0, 1, to_string(core_percentage_size).c_str(), 1)){
			int frameBufferWidth, frameBufferHeight;
			glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);
			post_resize(frameBufferWidth, frameBufferHeight);
		}
	}

	if (ImGui::Combo("View", (int *)(&view), "Horizontal\0Vertical\0InputOnly\0OutputOnly\0\0")) {
		// That's how you get the current width/height of the frame buffer (for example, after the window was resized)
		int frameBufferWidth, frameBufferHeight;
		glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);
		post_resize(frameBufferWidth, frameBufferHeight);
	}

	if(ImGui::Combo("Mouse Mode", (int *)(&mouse_mode), "NONE\0FACE_SELECT\0VERTEX_SELECT\0CLEAR\0\0")) {
		if (mouse_mode == CLEAR) {
			selected_faces.clear();
			selected_vertices.clear();
			UpdateHandles();
		}
	}

	Draw_menu_for_Solver();
	Draw_menu_for_cores();
	Draw_menu_for_models();
	Draw_menu_for_colors();
	Draw_menu_for_text_results();

	follow_and_mark_selected_faces();
	Update_view();
}

IGL_INLINE void BasicMenu::post_resize(int w, int h)
{
	if (viewer)
	{
		if (view == Horizontal) {
			viewer->core(input_view_id).viewport = Vector4f(0, 0, w * core_percentage_size, h);
			viewer->core(output_view_id).viewport = Vector4f(w * core_percentage_size, 0, w - (w * core_percentage_size), h);
		}
		if (view == Vertical) {
			viewer->core(input_view_id).viewport = Vector4f(0, h * core_percentage_size, w, h - (h * core_percentage_size));
			viewer->core(output_view_id).viewport = Vector4f(0, 0, w, h * core_percentage_size);
		}
		if (view == InputOnly) {
			viewer->core(input_view_id).viewport = Vector4f(0, 0, w, h);
			viewer->core(output_view_id).viewport = Vector4f(w + 1, h + 1, w + 2, h + 2);
		}
		if (view == OutputOnly) {
			viewer->core(input_view_id).viewport = Vector4f(w + 1, h + 1, w + 2, h + 2);
			viewer->core(output_view_id).viewport = Vector4f(0, 0, w, h);
		}
	}
}

IGL_INLINE bool BasicMenu::mouse_move(int mouse_x, int mouse_y)
{
	if (!IsTranslate)
	{
		return false;
	}
	if (mouse_mode == FACE_SELECT)
	{
		if (!selected_faces.empty())
		{
			RowVector3d face_avg_pt = get_face_avg();
			RowVector3i face = viewer->data(Model_Translate_ID).F.row(Translate_Index);

			Vector3f translation = computeTranslation(mouse_x, down_mouse_x, mouse_y, down_mouse_y, face_avg_pt);
			viewer->data(Model_Translate_ID).V.row(face[0]) += translation.cast<double>();
			viewer->data(Model_Translate_ID).V.row(face[1]) += translation.cast<double>();
			viewer->data(Model_Translate_ID).V.row(face[2]) += translation.cast<double>();

			viewer->data(Model_Translate_ID).set_mesh(viewer->data(Model_Translate_ID).V, viewer->data(Model_Translate_ID).F);
			down_mouse_x = mouse_x;
			down_mouse_y = mouse_y;
			UpdateHandles();
			return true;
		}
	}
	else if (mouse_mode == VERTEX_SELECT)
	{
		if (!selected_vertices.empty())
		{
			RowVector3d vertex_pos = viewer->data(Model_Translate_ID).V.row(Translate_Index);
			Vector3f translation = computeTranslation(mouse_x, down_mouse_x, mouse_y, down_mouse_y, vertex_pos);
			viewer->data(Model_Translate_ID).V.row(Translate_Index) += translation.cast<double>();

			viewer->data(Model_Translate_ID).set_mesh(viewer->data(Model_Translate_ID).V, viewer->data(Model_Translate_ID).F);
			down_mouse_x = mouse_x;
			down_mouse_y = mouse_y;
			UpdateHandles();
			return true;
		}
	}
	UpdateHandles();
	return false;
}

IGL_INLINE bool BasicMenu::mouse_up(int button, int modifier) {
	IsTranslate = false;
	return false;
}

IGL_INLINE bool BasicMenu::mouse_down(int button, int modifier) {
	down_mouse_x = viewer->current_mouse_x;
	down_mouse_y = viewer->current_mouse_y;
			
	if (mouse_mode == FACE_SELECT && button == GLFW_MOUSE_BUTTON_LEFT && modifier == 2)
	{
		//check if there faces which is selected on the left screen
		int f = pick_face(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, InputOnly);
		if (f == -1) {
			//check if there faces which is selected on the right screen
			f = pick_face(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, OutputOnly);
		}

		if (f != -1)
		{
			if (find(selected_faces.begin(), selected_faces.end(), f) != selected_faces.end())
			{
				selected_faces.erase(f);
				UpdateHandles();
			}
			else {
				selected_faces.insert(f);
				UpdateHandles();
			}
		}

	}
	else if (mouse_mode == VERTEX_SELECT && button == GLFW_MOUSE_BUTTON_LEFT && modifier == 2)
	{
		//check if there faces which is selected on the left screen
		int v = pick_vertex(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, InputOnly);
		if (v == -1) {
			//check if there faces which is selected on the right screen
			v = pick_vertex(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, OutputOnly);
		}

		if (v != -1)
		{
			if (find(selected_vertices.begin(), selected_vertices.end(), v) != selected_vertices.end())
			{
				selected_vertices.erase(v);
				UpdateHandles();
			}
			else {
				selected_vertices.insert(v);
				UpdateHandles();
			}
					
		}
	}
	else if (mouse_mode == FACE_SELECT && button == GLFW_MOUSE_BUTTON_MIDDLE && modifier == 2)
	{
		if (!selected_faces.empty())
		{
			//check if there faces which is selected on the left screen
			int f = pick_face(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, InputOnly);
			Model_Translate_ID = InputModelID();
			Core_Translate_ID = input_view_id;
			if (f == -1) {
				//check if there faces which is selected on the right screen
				f = pick_face(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, OutputOnly);
				Model_Translate_ID = OutputModelID();
				Core_Translate_ID = output_view_id;
			}

			if (find(selected_faces.begin(), selected_faces.end(), f) != selected_faces.end())
			{
				IsTranslate = true;
				Translate_Index = f;
			}
		}
	}
	else if (mouse_mode == VERTEX_SELECT && button == GLFW_MOUSE_BUTTON_MIDDLE && modifier == 2)
	{
		if (!selected_vertices.empty())
		{
			//check if there faces which is selected on the left screen
			int v = pick_vertex(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, InputOnly);
			Model_Translate_ID = InputModelID();
			Core_Translate_ID = input_view_id;
			if (v == -1) {
				//check if there faces which is selected on the right screen
				v = pick_vertex(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, OutputOnly);
				Model_Translate_ID = OutputModelID();
				Core_Translate_ID = output_view_id;
			}

			if (find(selected_vertices.begin(), selected_vertices.end(), v) != selected_vertices.end())
			{
				IsTranslate = true;
				Translate_Index = v;
			}
		}
	}

	return false;
}

IGL_INLINE bool BasicMenu::key_pressed(unsigned int key, int modifiers) {
	
	if (key == 'F' || key == 'f') {
		mouse_mode = FACE_SELECT;
	}
	if (key == 'V' || key == 'v') {
		mouse_mode = VERTEX_SELECT;
	}
	if (key == 'C' || key == 'c') {
		mouse_mode = CLEAR;
		selected_faces.clear();
		selected_vertices.clear();
		UpdateHandles();
	}
	return ImGuiMenu::key_pressed(key, modifiers);
}

IGL_INLINE void BasicMenu::shutdown()
{
	stop_solver_thread();
	ImGuiMenu::shutdown();
}

IGL_INLINE bool BasicMenu::pre_draw() {
	//call parent function
	ImGuiMenu::pre_draw();
			
	if (solver->progressed)
		update_mesh();
	
	//Update the model's faces colors in the two screens
	if (color_per_face.size()) {
		viewer->data(InputModelID()).set_colors(color_per_face);
		viewer->data(OutputModelID()).set_colors(color_per_face);
	}
			
	//Update the model's vertex colors in the two screens
	viewer->data(InputModelID()).point_size = 10;
	viewer->data(OutputModelID()).point_size = 10;
		
	viewer->data(InputModelID()).set_points(Vertices_Input, color_per_vertex);
	viewer->data(OutputModelID()).set_points(Vertices_output, color_per_vertex);

	return false;
}

void BasicMenu::Draw_menu_for_colors() {
	if (!ImGui::CollapsingHeader("colors", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit3("Highlighted face color", Highlighted_face_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Fixed face color", Fixed_face_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Dragged face color", Dragged_face_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Fixed vertex color", Fixed_vertex_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Dragged vertex color", Dragged_vertex_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Model color", model_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit3("Vertex Energy color", Vertex_Energy_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
		ImGui::ColorEdit4("text color", text_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
	}
}

void BasicMenu::Draw_menu_for_Solver() {
	if (!ImGui::CollapsingHeader("Solver", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox(solver_on ? "On" : "Off", &solver_on)) {
			if (solver_on) {
				start_solver_thread();
			}
			else {
				stop_solver_thread();
			}
		}

		ImGui::Combo("Dist check", (int *)(&distortion_type), "NO_DISTORTION\0AREA_DISTORTION\0TOTAL_DISTORTION\0\0");
		
		Parametrization prev_type = param_type;
		if (ImGui::Combo("Initial Guess", (int *)(&param_type), "RANDOM\0HARMONIC\0LSCM\0ARAP\0NONE\0\0")) {
			MatrixXd initialguess;
			MatrixX3i F = viewer->data(OutputModelID()).F;
			Parametrization temp = param_type;
			param_type = prev_type;
			if (temp == None) {
				param_type = None;
			}
			else if (temp == HARMONIC && !IsMesh2D() && F.size()) {
				initialguess = compute_harmonic_param();
				VectorXd initialguessXX = Map<const VectorXd>(initialguess.data(), initialguess.rows() * 2);
				solver->init(totalObjective, initialguessXX);
				solver->setFlipAvoidingLineSearch(F);
			}
			else if (temp == LSCM && !IsMesh2D() && F.size()) {
				initialguess = compute_lscm_param();
				VectorXd initialguessXX = Map<const VectorXd>(initialguess.data(), initialguess.rows() * 2);
				solver->init(totalObjective, initialguessXX);
				solver->setFlipAvoidingLineSearch(F);
			}
			else if (temp == ARAP && !IsMesh2D() && F.size()) {
				initialguess = compute_ARAP_param();
				VectorXd initialguessXX = Map<const VectorXd>(initialguess.data(), initialguess.rows() * 2);
				solver->init(totalObjective, initialguessXX);
				solver->setFlipAvoidingLineSearch(F);
			}
			else if (temp == RANDOM && F.size()) {
				initialguess = ComputeSoup2DRandom();
				VectorXd initialguessXX = Map<const VectorXd>(initialguess.data(), initialguess.rows() * 2);
				solver->init(totalObjective, initialguessXX);
				solver->setFlipAvoidingLineSearch(F);
			}
		}
		float w = ImGui::GetContentRegionAvailWidth(), p = ImGui::GetStyle().FramePadding.x;
		if (ImGui::Button("Check gradients", ImVec2((w - p) / 2.f, 0)))
		{
			checkGradients();
		}
		ImGui::SameLine(0, p);
		if (ImGui::Button("Check Hessians", ImVec2((w - p) / 2.f, 0)))
		{
			checkHessians();
		}
		
		ImGui::DragFloat("Max Distortion", &(Max_Distortion), 0.05f, 0.1f, 20.0f);
		

		// objective functions wieghts
		int id = 0;
		for (auto& obj : totalObjective->objectiveList) {
			ImGui::PushID(id++);
			ImGui::Text(obj->name);
			ImGui::PushItemWidth(80 * menu_scaling());
			ImGui::DragFloat("weight", &(obj->w) , 0.05f, 0.1f, 20.0f);
			ImGui::PushItemWidth(80 * menu_scaling());
			ImGui::DragFloat("shift eigen values", &(obj->Shift_eigen_values), 0.07f, 0.1f, 20.0f);
			ImGui::PopID();
		}
	}
}

void BasicMenu::Draw_menu_for_cores() {
	for (auto& core : viewer->core_list)
	{
		ImGui::PushID(core.id);
		stringstream ss;
		string name = (core.id == input_view_id) ? "Input Core" : "Output Core";
		ss << name;
		if (!ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			int data_id = OutputModelID();
			if (core.id == input_view_id) {
				data_id = InputModelID();
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
			static Quaternionf trackball_angle = Quaternionf::Identity();
			static bool orthographic = true;
			if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
			{
				using RT = ViewerCore::RotationType;
				auto new_type = static_cast<RT>(rotation_type);
				if (new_type != core.rotation_type)
				{
					if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
					{
						trackball_angle = core.trackball_angle;
						orthographic = core.orthographic;
						core.trackball_angle = Quaternionf::Identity();
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
		ImGui::PopID();
	}
}

void BasicMenu::Draw_menu_for_models() {
	for (auto& data : viewer->data_list)
	{
		// Helper for setting viewport specific mesh options
		auto make_checkbox = [&](const char *label, unsigned int &option)
		{
			bool temp = option;
			bool res = ImGui::Checkbox(label, &temp);
			option = temp;
			return res;
		};

		ImGui::PushID(data.id);
		stringstream ss;

		if (data_id_to_name.count(data.id) > 0)
		{
			ss << data_id_to_name[data.id];
		}
		else
		{
			ss << "Data " << data.id;
		}

		if (!ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			float w = ImGui::GetContentRegionAvailWidth();
			float p = ImGui::GetStyle().FramePadding.x;

			if (ImGui::Button("texture x2", ImVec2((w - p) / 2.f, 0))) {
				data.set_uv(data.V_uv * 2);
			}
			ImGui::SameLine(0, p);
			if (ImGui::Button("texture /2", ImVec2((w - p) / 2.f, 0))) {
				data.set_uv(data.V_uv / 2);
			}

			if (ImGui::Checkbox("Face-based", &(data.face_based)))
			{
				data.dirty = MeshGL::DIRTY_ALL;
			}

			make_checkbox("Show texture", data.show_texture);
			if (ImGui::Checkbox("Invert normals", &(data.invert_normals)))
			{
				data.dirty |= MeshGL::DIRTY_NORMAL;
			}
			make_checkbox("Show overlay", data.show_overlay);
			make_checkbox("Show overlay depth", data.show_overlay_depth);
			ImGui::ColorEdit4("Line color", data.line_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
			ImGui::DragFloat("Shininess", &(data.shininess), 0.05f, 0.0f, 100.0f);
			ImGui::PopItemWidth();

			make_checkbox("Wireframe", data.show_lines);
			make_checkbox("Fill", data.show_faces);
			ImGui::Checkbox("Show vertex labels", &(data.show_vertid));
			ImGui::Checkbox("Show faces labels", &(data.show_faceid));
		}
		ImGui::PopID();
	}
}

void BasicMenu::Draw_menu_for_text_results() {
	if (!show_text) {
		return;
	}

	int frameBufferWidth, frameBufferHeight;
	float shift = ImGui::GetTextLineHeightWithSpacing();
	glfwGetFramebufferSize(viewer->window, &frameBufferWidth, &frameBufferHeight);

	int w, h;
	if (view == Horizontal) {
		w = frameBufferWidth * core_percentage_size + shift;
		h = shift;
	}
	if (view == Vertical) {
		w = shift;
		h = frameBufferHeight - frameBufferHeight * core_percentage_size + shift;
	}
	if (view == InputOnly) {
		w = frameBufferWidth * core_percentage_size + shift;
		h = shift;
	}
	if (view == OutputOnly) {
		w = frameBufferWidth * core_percentage_size + shift;
		h = shift;
	}


	bool bOpened(true);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("BCKGND", &bOpened, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetWindowCollapsed(false);
	ImColor c(text_color[0], text_color[1], text_color[2], 1.0f);

	//add text...
	ImGui::GetWindowDrawList()->AddText(ImVec2(w, h), c, (std::string(totalObjective->name) + std::string(" energy ") + std::to_string(totalObjective->energy_value)).c_str());
	h += shift;
	ImGui::GetWindowDrawList()->AddText(ImVec2(w, h), c, (std::string(totalObjective->name) + std::string(" gradient ") + std::to_string(totalObjective->gradient_norm)).c_str());
	h += shift;

	for (auto& obj : totalObjective->objectiveList) {
		ImGui::GetWindowDrawList()->AddText(ImVec2(w, h), c, (std::string(obj->name) + std::string(" energy ") + std::to_string(obj->energy_value)).c_str());
		h += shift;
		ImGui::GetWindowDrawList()->AddText(ImVec2(w, h), c, (std::string(obj->name) + std::string(" gradient ") + std::to_string(obj->gradient_norm)).c_str());
		h += shift;
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

void BasicMenu::UpdateHandles() {
	vector<int> CurrHandlesInd;
	MatrixX2d CurrHandlesPosDeformed;
	CurrHandlesInd.clear();

	//First, we push each vertices index to the handles
	for (auto vi : selected_vertices) {
		CurrHandlesInd.push_back(vi);
	}
	//Then, we push each face vertices index to the handle (3 vertices)
	for (auto fi : selected_faces) {
		//Here we get the 3 vertice's index that build each face
		int v0 = viewer->data(OutputModelID()).F(fi,0);
		int v1 = viewer->data(OutputModelID()).F(fi,1);
		int v2 = viewer->data(OutputModelID()).F(fi,2);

		//check whether the handle already exist
		if (!(find(CurrHandlesInd.begin(), CurrHandlesInd.end(), v0) != CurrHandlesInd.end())){
			CurrHandlesInd.push_back(v0);
		}

		if (!(find(CurrHandlesInd.begin(), CurrHandlesInd.end(), v1) != CurrHandlesInd.end())) {
			CurrHandlesInd.push_back(v1);
		}

		if (!(find(CurrHandlesInd.begin(), CurrHandlesInd.end(), v2) != CurrHandlesInd.end())) {
			CurrHandlesInd.push_back(v2);
		}
	}
			
	//Here we update the positions for each handle
	CurrHandlesPosDeformed.resize(CurrHandlesInd.size(),2);
	int idx = 0;
	for (auto hi : CurrHandlesInd) {
		CurrHandlesPosDeformed.row(idx++) << viewer->data(OutputModelID()).V(hi, 0), viewer->data(OutputModelID()).V(hi, 1);
	}
	
	//Update texture
	update_texture(viewer->data(OutputModelID()).V);

	//Finally, we update the handles in the constraints positional object
	if (solverInitialized) {
		(*HandlesInd) = CurrHandlesInd;
		(*HandlesPosDeformed) = CurrHandlesPosDeformed;
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
	viewer->data(InputModelID()).set_visible(true, input_view_id);
	viewer->data(OutputModelID()).set_visible(true, output_view_id);
}

void BasicMenu::follow_and_mark_selected_faces() {
	//check if there faces which is selected on the left screen
	int f = pick_face(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, InputOnly);
	if (f == -1) {
		//check if there faces which is selected on the right screen
		f = pick_face(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, OutputOnly);
	}
	
	if(viewer->data(InputModelID()).F.size()){
		//Mark the faces
		color_per_face.resize(viewer->data(InputModelID()).F.rows(), 3);
		UpdateEnergyColors();
		//Mark the fixed faces
		if (f != -1 && Highlighted_face)
		{
			color_per_face.row(f) = Highlighted_face_color.cast<double>();
		}
		for (auto fi : selected_faces) { color_per_face.row(fi) = Fixed_face_color.cast<double>(); }
		//Mark the Dragged face
		if (IsTranslate && (mouse_mode == FACE_SELECT)) {
			color_per_face.row(Translate_Index) = Dragged_face_color.cast<double>();
		}
		
		//Mark the vertices
		int idx = 0;
		Vertices_Input.resize(selected_vertices.size(), 3);
		Vertices_output.resize(selected_vertices.size(), 3);
		color_per_vertex.resize(selected_vertices.size(), 3);
		//Mark the dragged vertex
		if (IsTranslate && (mouse_mode == VERTEX_SELECT)) {
			Vertices_Input.resize(selected_vertices.size()+1, 3);
			Vertices_output.resize(selected_vertices.size()+1, 3);
			color_per_vertex.resize(selected_vertices.size()+1, 3);

			Vertices_Input.row(idx) = viewer->data(InputModelID()).V.row(Translate_Index);
			color_per_vertex.row(idx) = Dragged_vertex_color.cast<double>();
			Vertices_output.row(idx) = viewer->data(OutputModelID()).V.row(Translate_Index);
			idx++;
		}
				
		//Mark the fixed vertices
		for (auto vi : selected_vertices) {
			Vertices_Input.row(idx) = viewer->data(InputModelID()).V.row(vi);
			Vertices_output.row(idx) = viewer->data(OutputModelID()).V.row(vi);
			color_per_vertex.row(idx++) = Fixed_vertex_color.cast<double>();
		}
	}
}
	
void BasicMenu::set_name_mapping(unsigned int data_id, string name)
{
	data_id_to_name[data_id] = name;
	data_id_to_name[data_id+1] = name + " (Param.)";
}

int BasicMenu::InputModelID() {
	return 0;
}

int BasicMenu::OutputModelID() {
	return 1;
}

bool BasicMenu::IsMesh2D() {
	MatrixXd V = viewer->data(InputModelID()).V;
	return (V.col(2).array() == 0).all();
}

char* BasicMenu::getModelNames()
{
	string cStr = "";
	for (auto& data : viewer->data_list)
	{
		stringstream ss;
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

RowVector3d BasicMenu::get_face_avg() {
	RowVector3d avg; avg << 0, 0, 0;
	RowVector3i face = viewer->data(Model_Translate_ID).F.row(Translate_Index);

	avg += viewer->data(Model_Translate_ID).V.row(face[0]);
	avg += viewer->data(Model_Translate_ID).V.row(face[1]);
	avg += viewer->data(Model_Translate_ID).V.row(face[2]);
	avg /= 3;

	return avg;
}

Vector3f BasicMenu::computeTranslation(int mouse_x, int from_x, int mouse_y, int from_y, RowVector3d pt3D) {
	Matrix4f modelview = viewer->core(Core_Translate_ID).view;
	//project the given point (typically the handle centroid) to get a screen space depth
	Vector3f proj = project(pt3D.transpose().cast<float>().eval(),
		modelview,
		viewer->core(Core_Translate_ID).proj,
		viewer->core(Core_Translate_ID).viewport);
	float depth = proj[2];

	double x, y;
	Vector3f pos1, pos0;

	//unproject from- and to- points
	x = mouse_x;
	y = viewer->core(Core_Translate_ID).viewport(3) - mouse_y;
	pos1 = unproject(Vector3f(x, y, depth),
		modelview,
		viewer->core(Core_Translate_ID).proj,
		viewer->core(Core_Translate_ID).viewport);


	x = from_x;
	y = viewer->core(Core_Translate_ID).viewport(3) - from_y;
	pos0 = unproject(Vector3f(x, y, depth),
		modelview,
		viewer->core(Core_Translate_ID).proj,
		viewer->core(Core_Translate_ID).viewport);

	//translation is the vector connecting the two
	Vector3f translation;
	translation = pos1 - pos0;

	return translation;
}

int BasicMenu::pick_face(MatrixXd& V,MatrixXi& F, View LR) {
	// Cast a ray in the view direction starting from the mouse position
	int core_index;
	if (LR == OutputOnly) {
		core_index = output_view_id;
	}
	else if (LR == InputOnly) {
		core_index = input_view_id;
	}
	double x = viewer->current_mouse_x;
	double y = viewer->core(core_index).viewport(3) - viewer->current_mouse_y;
	if (view == Vertical) {
		y = (viewer->core(input_view_id).viewport(3) / core_percentage_size) - viewer->current_mouse_y;
	}


	RowVector3d pt;

	Matrix4f modelview = viewer->core(core_index).view;
	int vi = -1;

	vector<Hit> hits;

	unproject_in_mesh(Vector2f(x, y), viewer->core(core_index).view,
		viewer->core(core_index).proj, viewer->core(core_index).viewport, V, F, pt, hits);

	int fi = -1;
	if (hits.size() > 0) {
		fi = hits[0].id;
	}
	return fi;
}

int BasicMenu::pick_vertex(MatrixXd& V, MatrixXi& F,View LR) {
	// Cast a ray in the view direction starting from the mouse position
	int core_index;
	if (LR == OutputOnly) {
		core_index = output_view_id;
	}
	else if (LR == InputOnly) {
		core_index = input_view_id;
	}

	double x = viewer->current_mouse_x;
	double y = viewer->core(core_index).viewport(3) - viewer->current_mouse_y;
	if (view == Vertical) {
		y = (viewer->core(input_view_id).viewport(3) / core_percentage_size) - viewer->current_mouse_y;
	}

	RowVector3d pt;

	Matrix4f modelview = viewer->core(core_index).view;
	int vi = -1;

	vector<Hit> hits;
			
	unproject_in_mesh(Vector2f(x, y), viewer->core(core_index).view,
		viewer->core(core_index).proj, viewer->core(core_index).viewport, V, F, pt, hits);

	if (hits.size() > 0) {
		int fi = hits[0].id;
		RowVector3d bc;
		bc << 1.0 - hits[0].u - hits[0].v, hits[0].u, hits[0].v;
		bc.maxCoeff(&vi);
		vi = F(fi, vi);
	}
	return vi;
}
	
MatrixXd BasicMenu::compute_ARAP_param() {
	param_type = ARAP;
	// Compute the initial solution for ARAP (harmonic parametrization)
	VectorXi bnd;
	MatrixXd V_uv, initial_guess;

	boundary_loop(viewer->data(InputModelID()).F, bnd);
	MatrixXd bnd_uv;
	map_vertices_to_circle(viewer->data(InputModelID()).V, bnd, bnd_uv);

	harmonic(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, bnd, bnd_uv, 1, initial_guess);

	// Add dynamic regularization to avoid to specify boundary conditions
	ARAPData arap_data;
	arap_data.with_dynamics = true;
	VectorXi b = VectorXi::Zero(0);
	MatrixXd bc = MatrixXd::Zero(0, 0);

	// Initialize ARAP
	arap_data.max_iter = 100;
	// 2 means that we're going to *solve* in 2d
	arap_precomputation(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, 2, b, arap_data);

	// Solve arap using the harmonic map as initial guess
	V_uv = initial_guess;

	arap_solve(bc, arap_data, V_uv);
	
	update_texture(V_uv);
	Update_view();
	return V_uv;
}

MatrixXd BasicMenu::compute_harmonic_param() {
	param_type = HARMONIC;
	// Find the open boundary
	VectorXi bnd;
	MatrixXd V_uv;
	boundary_loop(viewer->data(InputModelID()).F, bnd);

	// Map the boundary to a circle, preserving edge proportions
	MatrixXd bnd_uv;
	map_vertices_to_circle(viewer->data(InputModelID()).V, bnd, bnd_uv);

	harmonic(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, bnd, bnd_uv, 1, V_uv);
	
	update_texture(V_uv);
	Update_view();
	return V_uv;
}

MatrixXd BasicMenu::compute_lscm_param()
{
	param_type = LSCM;
	// Fix two points on the boundary
	VectorXi bnd, b(2, 1);
	MatrixXd V_uv;
	boundary_loop(viewer->data(InputModelID()).F, bnd);
	b(0) = bnd(0);
	b(1) = bnd(round(bnd.size() / 2));
	MatrixXd bc(2, 2);
	bc << 0, 0, 1, 0;

	lscm(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, b, bc, V_uv);
	
	update_texture(V_uv);
	Update_view();
	return V_uv;
}

MatrixXd BasicMenu::ComputeSoup2DRandom()
{
	param_type = RANDOM;
	MatrixXd V_uv;
	auto nvs = viewer->data(InputModelID()).V.rows();
	V_uv = MatrixX2d::Random(nvs, 2) * 2.0;
	//FixFlippedFaces(viewer->data(InputModelID()).F, V_uv);

	update_texture(V_uv);
	Update_view();
	return V_uv;
}

void BasicMenu::update_texture(MatrixXd& V_uv) {
	MatrixXd V_uv_2D(V_uv.rows(),2);
	MatrixXd V_uv_3D(V_uv.rows(),3);
	if (V_uv.cols() == 2) {
		V_uv_2D = V_uv;
		V_uv_3D.leftCols(2) = V_uv.leftCols(2);
		V_uv_3D.rightCols(1).setZero();
	}
	else if (V_uv.cols() == 3) {
		V_uv_3D = V_uv;
		V_uv_2D = V_uv.leftCols(2);
	}

	// Plot the mesh
	viewer->data(InputModelID()).set_uv(V_uv_2D);
	viewer->data(OutputModelID()).set_vertices(V_uv_3D);
	viewer->data(OutputModelID()).set_uv(V_uv_2D);
	viewer->data(OutputModelID()).compute_normals();
}
	
void BasicMenu::checkGradients()
{
	if (!solverInitialized) {
		solver_on = false;
		return;
	}
	stop_solver_thread();
	for (auto const &objective : totalObjective->objectiveList) {
		objective->checkGradient(solver->ext_x);
	}
	start_solver_thread();
}

void BasicMenu::checkHessians()
{
	if (!solverInitialized) {
		solver_on = false;
		return;
	}
	stop_solver_thread();
	for (auto const &objective : totalObjective->objectiveList) {
		objective->checkHessian(solver->ext_x);
	}
	start_solver_thread();
}

void BasicMenu::start_solver_thread() {
	if(!solverInitialized){
		solver_on = false;
		return;
	}
	cout << ">> start new solver" << endl;
	solver_on = true;
	solver_thread = thread(&Solver::run, solver.get());
	solver_thread.detach();
}

void BasicMenu::stop_solver_thread() {
	solver_on = false;
	if (solver->is_running) {
		solver->stop();
	}
	while (solver->is_running);
}

void BasicMenu::update_mesh()
{
	VectorXd X;
	solver->get_data(X);
	MatrixXd V(X.rows() / 2, 2);
	V = Map<MatrixXd>(X.data(), X.rows() / 2, 2);
	
	update_texture(V);
}

void BasicMenu::initializeSolver()
{
	//Check point for 2D mesh
	//compute_harmonic_param();

	MatrixXd V = viewer->data(OutputModelID()).V;
	MatrixX3i F = viewer->data(OutputModelID()).F;
	
	stop_solver_thread();

	if (V.rows() == 0 || F.rows() == 0)
		return;

	// initialize the energy
	auto symDirichlet = make_unique<ObjectiveSymmetricDirichlet>();
	symDirichlet->setVF(V, F);
	symDirichlet->init();
	auto areaPreserving = make_unique<ObjectiveAreaPreserving>();
	areaPreserving->setVF(V, F);
	areaPreserving->init();
	
	constraintsPositional->numV = V.rows();
	constraintsPositional->init();
	HandlesInd = &constraintsPositional->ConstrainedVerticesInd;
	HandlesPosDeformed = &constraintsPositional->ConstrainedVerticesPos;

	totalObjective->objectiveList.clear();
	totalObjective->objectiveList.push_back(move(areaPreserving));
	totalObjective->objectiveList.push_back(move(symDirichlet));
	totalObjective->objectiveList.push_back(move(constraintsPositional));

	totalObjective->init();

	// initialize the solver
	MatrixXd initialguess;
	if (IsMesh2D()) {
		//the mesh is 2D
		initialguess = V;
	}
	else {
		//the mesh is 3D
		initialguess = compute_harmonic_param();
	}
	VectorXd initialguessXX = Map<const VectorXd>(initialguess.data(), initialguess.rows() * 2);
	solver->init(totalObjective, initialguessXX);
	solver->setFlipAvoidingLineSearch(F);
	
	cout << "Solver is initialized!" << endl;
	solverInitialized = true;
}

void BasicMenu::FixFlippedFaces(MatrixXi& Fs, MatrixXd& Vs)
{
	Matrix<double, 3, 2> face_vertices;
	for (MatrixXi::Index i = 0; i < Fs.rows(); ++i)
	{
		slice(Vs, Fs.row(i), 1, face_vertices);
		Vector2d v1_2d = face_vertices.row(1) - face_vertices.row(0);
		Vector2d v2_2d = face_vertices.row(2) - face_vertices.row(0);
		Vector3d v1_3d = Vector3d(v1_2d.x(), v1_2d.y(), 0);
		Vector3d v2_3d = Vector3d(v2_2d.x(), v2_2d.y(), 0);
		Vector3d face_normal = v1_3d.cross(v2_3d);

		// If face is flipped (that is, cross-product do not obey the right-hand rule)
		if (face_normal(2) < 0)
		{	
			Fs.row(i) << Fs(i, 0), Fs(i, 2), Fs(i, 1);
		}
	}
}

void BasicMenu::UpdateEnergyColors() {
	int numF = viewer->data(OutputModelID()).F.rows();
	VectorXd DistortionPerFace(numF);
	DistortionPerFace.setZero();
	VectorXd ones(numF);
	ones.setOnes();

	if (distortion_type == AREA_DISTORTION) {	//distortion according to area preserving
		VectorXd Area_output, Area_input;
		igl::doublearea(viewer->data(OutputModelID()).V, viewer->data(OutputModelID()).F, Area_output);
		igl::doublearea(viewer->data(InputModelID()).V, viewer->data(InputModelID()).F, Area_input);
		// DistortionPerFace = Area_output / Area_input
		DistortionPerFace = Area_input.cwiseInverse().cwiseProduct(Area_output);
		// Becuase we want  DistortionPerFace to be as colse as possible to zero instead of one!
		DistortionPerFace = DistortionPerFace - ones;
	}
	else if(distortion_type == TOTAL_DISTORTION) {
		// calculate the distortion over all the energies
		for (auto& obj : totalObjective->objectiveList) 
			if ((obj->Efi.size() != 0) && (obj->w != 0)) 
				DistortionPerFace += obj->Efi * obj->w;
	}

	
	VectorXd alpha_vec = DistortionPerFace / Max_Distortion;
	VectorXd beta_vec = ones - alpha_vec;
	MatrixXd alpha(numF, 3), beta(numF, 3);
	alpha = alpha_vec.replicate(1, 3);
	beta = beta_vec.replicate(1, 3);

	//calculate low distortion color matrix
	MatrixXd LowDistCol = model_color.cast <double>().replicate(1, numF).transpose();
	//calculate high distortion color matrix
	MatrixXd HighDistCol = Vertex_Energy_color.cast <double>().replicate(1, numF).transpose();
	
	color_per_face = beta.cwiseProduct(LowDistCol) + alpha.cwiseProduct(HighDistCol);
}
