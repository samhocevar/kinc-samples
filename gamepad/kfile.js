const project = new Project('gamepad');

if (!process.env.KINC_SDK_PATH) throw new Error('Environment variable KINC_SDK_PATH not found');
await project.addProject(process.env.KINC_SDK_PATH);

project.addFile('*.c');
project.addFile('*.glsl');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
