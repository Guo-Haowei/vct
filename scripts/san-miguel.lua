-- scene
Dvar.SetString('scene', '../assets/San_Miguel/san-miguel-low-poly.obj');
Dvar.SetFloat('scene_scale', 0.5);

local scale = 2;
-- window
Dvar.SetInt('wnd_width', 1920 * scale);
Dvar.SetInt('wnd_height', 1080 * scale);
Dvar.SetInt('wnd_frameless', 1);

-- graphics
Dvar.SetInt('r_shadowRes', 1024 * 8 * 2);
Dvar.SetInt('r_voxel_size', 64);

-- camera
Dvar.SetVec3('cam_pos', 12.54, 0.95, 0.86);
Dvar.SetFloat('cam_fov', 60.0);

-- light
Dvar.SetVec3('light_dir', -4.0, 25.0, -1.8);