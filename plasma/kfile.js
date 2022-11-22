const project = new Project('plasma');

await project.addProject('../../kinc/');

project.addFile('*.c');
project.addFile('*.glsl');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
