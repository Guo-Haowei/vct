local windowScale = 1;

-- scene
Dvar.SetString('scene', 'data/models/Sponza/sponza.gltf');
Dvar.SetFloat('scene_scale', 0.01);

-- window
Dvar.SetInt('wnd_width', 1920 * windowScale);
Dvar.SetInt('wnd_height', 1080 * windowScale);
-- Dvar.SetInt('wnd_frameless', 1);
Dvar.SetInt('r_voxelSize', 64);

-- graphics
Dvar.SetInt('r_shadowRes', 1024 * 8);

-- camera
Dvar.SetVec3('cam_pos', -5.0, 5.0, 0.0);
-- Dvar.SetVec3('cam_pos', -5.6, 0.83, 2.72);
Dvar.SetVec4('cam_cascades', 0.1, 8.0, 18.0, 50.0);
Dvar.SetFloat('cam_fov', 60.0);