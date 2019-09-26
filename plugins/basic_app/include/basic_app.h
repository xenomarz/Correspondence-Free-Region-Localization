#pragma once

#ifndef RDS_PLUGINS_BASIC_MENU_H
#define RDS_PLUGINS_BASIC_MENU_H

#include "app_utils.h"
#include "core.h"

class basic_app : public ImGuiMenu
{
private:
	vector<Core> cores;
	//Basic (necessary) parameteres
	vector<int> output_view_id;
	int input_view_id;
	app_utils::View view;
	app_utils::MouseMode mouse_mode;
	
	float texture_scaling_input;
	
	bool IsTranslate;
	int Translate_Index, Model_Translate_ID, Core_Translate_ID, down_mouse_x, down_mouse_y;
	ImGuiMenu menu;

	

	// Solver thread
	thread solver_thread;

public:
	//Constructor & initialization
	basic_app();
	~basic_app(){}

	// callbacks
	IGL_INLINE virtual void draw_viewer_menu() override;
	IGL_INLINE virtual void init(Viewer *_viewer) override;
	IGL_INLINE virtual void post_resize(int w, int h) override;
	IGL_INLINE virtual bool mouse_move(int mouse_x, int mouse_y) override;
	IGL_INLINE virtual bool mouse_down(int button, int modifier) override;
	IGL_INLINE virtual bool mouse_up(int button, int modifier) override;
	IGL_INLINE virtual bool pre_draw() override;
	IGL_INLINE virtual void shutdown() override;
	IGL_INLINE virtual bool key_pressed(unsigned int key, int modifiers) override;
			
	//Draw menu methods
	void Draw_menu_for_cores();
	void Draw_menu_for_models();
	void Draw_menu_for_Solver();
	void Draw_menu_for_Energies();
	void Draw_menu_for_colors();
	void Draw_menu_for_text_results();

	//Pick faces & vertices and highlight them
	int pick_face(MatrixXd& V, MatrixXi& F, app_utils::View LR);
	int pick_vertex(MatrixXd& V, MatrixXi& F, app_utils::View LR);
	void follow_and_mark_selected_faces();
	RowVector3d get_face_avg();
	void UpdateHandles();
	void UpdateEnergyColors();

	//Basic Methods
	int InputModelID();
	ViewerData& InputModel();
	ViewerData& OutputModel();
	int OutputModelID();

	void Update_view();
	void update_mesh();
	void update_texture(MatrixXd& V_uv);

	//Start/Stop the solver Thread
	void start_solver_thread();
	void stop_solver_thread();
	void initializeSolver();

	//FD check
	void checkGradients();
	void checkHessians();
};

#endif