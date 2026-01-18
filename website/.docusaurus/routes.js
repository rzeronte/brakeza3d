import React from 'react';
import ComponentCreator from '@docusaurus/ComponentCreator';

export default [
  {
    path: '/brakeza3d/blog',
    component: ComponentCreator('/brakeza3d/blog', '1cd'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/archive',
    component: ComponentCreator('/brakeza3d/blog/archive', 'abc'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/authors',
    component: ComponentCreator('/brakeza3d/blog/authors', '4f8'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/bienvenida',
    component: ComponentCreator('/brakeza3d/blog/bienvenida', '249'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/tags',
    component: ComponentCreator('/brakeza3d/blog/tags', '2d6'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/tags/brakeza-3-d',
    component: ComponentCreator('/brakeza3d/blog/tags/brakeza-3-d', '2e4'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/tags/news',
    component: ComponentCreator('/brakeza3d/blog/tags/news', '8d9'),
    exact: true
  },
  {
    path: '/brakeza3d/blog/tags/welcome',
    component: ComponentCreator('/brakeza3d/blog/tags/welcome', 'b2b'),
    exact: true
  },
  {
    path: '/brakeza3d/contact',
    component: ComponentCreator('/brakeza3d/contact', 'c1c'),
    exact: true
  },
  {
    path: '/brakeza3d/downloads',
    component: ComponentCreator('/brakeza3d/downloads', '033'),
    exact: true
  },
  {
    path: '/brakeza3d/gallery',
    component: ComponentCreator('/brakeza3d/gallery', '39c'),
    exact: true
  },
  {
    path: '/brakeza3d/markdown-page',
    component: ComponentCreator('/brakeza3d/markdown-page', 'a27'),
    exact: true
  },
  {
    path: '/brakeza3d/docs',
    component: ComponentCreator('/brakeza3d/docs', 'ee1'),
    routes: [
      {
        path: '/brakeza3d/docs',
        component: ComponentCreator('/brakeza3d/docs', '49e'),
        routes: [
          {
            path: '/brakeza3d/docs',
            component: ComponentCreator('/brakeza3d/docs', 'e5e'),
            routes: [
              {
                path: '/brakeza3d/docs/advanced/building-from-sources',
                component: ComponentCreator('/brakeza3d/docs/advanced/building-from-sources', '6c9'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/advanced/core-shaders',
                component: ComponentCreator('/brakeza3d/docs/advanced/core-shaders', 'cfa'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/layout-modes',
                component: ComponentCreator('/brakeza3d/docs/editor/layout-modes', 'c30'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/overview',
                component: ComponentCreator('/brakeza3d/docs/editor/overview', 'a6e'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/projects',
                component: ComponentCreator('/brakeza3d/docs/editor/projects', 'e07'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/scenes',
                component: ComponentCreator('/brakeza3d/docs/editor/scenes', 'f52'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/scripts',
                component: ComponentCreator('/brakeza3d/docs/editor/scripts', '8aa'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/editor/shaders',
                component: ComponentCreator('/brakeza3d/docs/editor/shaders', '575'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/engine-concepts/assets-folder',
                component: ComponentCreator('/brakeza3d/docs/engine-concepts/assets-folder', '0de'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/engine-concepts/collisions',
                component: ComponentCreator('/brakeza3d/docs/engine-concepts/collisions', 'df1'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/engine-concepts/components',
                component: ComponentCreator('/brakeza3d/docs/engine-concepts/components', '43e'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/engine-concepts/global-illumination',
                component: ComponentCreator('/brakeza3d/docs/engine-concepts/global-illumination', '8b9'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/engine-concepts/script-controls',
                component: ComponentCreator('/brakeza3d/docs/engine-concepts/script-controls', '75c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/getting-started/download-and-install',
                component: ComponentCreator('/brakeza3d/docs/getting-started/download-and-install', 'c7c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/getting-started/intro',
                component: ComponentCreator('/brakeza3d/docs/getting-started/intro', '37c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/2d-objects',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/2d-objects', '085'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/3d-objects',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/3d-objects', '2ff'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/basic-object-types',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/basic-object-types', '102'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/camera-object',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/camera-object', '681'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/collision-system',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/collision-system', 'e91'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/components',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/components', 'a42'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/grid-and-octree',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/grid-and-octree', '46d'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/index',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/index', '7a6'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/input-system',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/input-system', '1a5'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/lighting-system',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/lighting-system', '168'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/particle-emitter',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/particle-emitter', 'fff'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/shaders-system',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/shaders-system', 'e89'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/brakeza3d/docs/scripting-api/sound-system',
                component: ComponentCreator('/brakeza3d/docs/scripting-api/sound-system', 'a59'),
                exact: true,
                sidebar: "tutorialSidebar"
              }
            ]
          }
        ]
      }
    ]
  },
  {
    path: '/brakeza3d/',
    component: ComponentCreator('/brakeza3d/', '099'),
    exact: true
  },
  {
    path: '*',
    component: ComponentCreator('*'),
  },
];
