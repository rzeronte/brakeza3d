import React from 'react';
import ComponentCreator from '@docusaurus/ComponentCreator';

export default [
  {
    path: '/assets-hub',
    component: ComponentCreator('/assets-hub', 'd59'),
    exact: true
  },
  {
    path: '/blog',
    component: ComponentCreator('/blog', '66f'),
    exact: true
  },
  {
    path: '/blog/2026-02-21-thanks-and-road-map',
    component: ComponentCreator('/blog/2026-02-21-thanks-and-road-map', 'a47'),
    exact: true
  },
  {
    path: '/blog/archive',
    component: ComponentCreator('/blog/archive', '182'),
    exact: true
  },
  {
    path: '/blog/assets-hub',
    component: ComponentCreator('/blog/assets-hub', '3ee'),
    exact: true
  },
  {
    path: '/blog/authors',
    component: ComponentCreator('/blog/authors', '0b7'),
    exact: true
  },
  {
    path: '/blog/brakeza3d-0-26-2',
    component: ComponentCreator('/blog/brakeza3d-0-26-2', 'e52'),
    exact: true
  },
  {
    path: '/blog/tags',
    component: ComponentCreator('/blog/tags', '287'),
    exact: true
  },
  {
    path: '/blog/tags/assets',
    component: ComponentCreator('/blog/tags/assets', 'ec3'),
    exact: true
  },
  {
    path: '/blog/tags/brakeza-3-d',
    component: ComponentCreator('/blog/tags/brakeza-3-d', 'fec'),
    exact: true
  },
  {
    path: '/blog/tags/community',
    component: ComponentCreator('/blog/tags/community', 'be0'),
    exact: true
  },
  {
    path: '/blog/tags/hub',
    component: ComponentCreator('/blog/tags/hub', 'f62'),
    exact: true
  },
  {
    path: '/blog/tags/news',
    component: ComponentCreator('/blog/tags/news', 'f2f'),
    exact: true
  },
  {
    path: '/blog/tags/release',
    component: ComponentCreator('/blog/tags/release', '52f'),
    exact: true
  },
  {
    path: '/blog/tags/roadmap',
    component: ComponentCreator('/blog/tags/roadmap', '781'),
    exact: true
  },
  {
    path: '/blog/tags/welcome',
    component: ComponentCreator('/blog/tags/welcome', 'cca'),
    exact: true
  },
  {
    path: '/blog/welcome',
    component: ComponentCreator('/blog/welcome', '978'),
    exact: true
  },
  {
    path: '/contact',
    component: ComponentCreator('/contact', '7ad'),
    exact: true
  },
  {
    path: '/downloads',
    component: ComponentCreator('/downloads', 'af8'),
    exact: true
  },
  {
    path: '/gallery',
    component: ComponentCreator('/gallery', '14e'),
    exact: true
  },
  {
    path: '/markdown-page',
    component: ComponentCreator('/markdown-page', '3d7'),
    exact: true
  },
  {
    path: '/docs',
    component: ComponentCreator('/docs', 'b96'),
    routes: [
      {
        path: '/docs',
        component: ComponentCreator('/docs', '08b'),
        routes: [
          {
            path: '/docs',
            component: ComponentCreator('/docs', 'ca2'),
            routes: [
              {
                path: '/docs/advanced/core-shaders',
                component: ComponentCreator('/docs/advanced/core-shaders', '1a7'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/layout-modes',
                component: ComponentCreator('/docs/editor/layout-modes', '477'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/node-editor',
                component: ComponentCreator('/docs/editor/node-editor', '153'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/overview',
                component: ComponentCreator('/docs/editor/overview', '73d'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/projects',
                component: ComponentCreator('/docs/editor/projects', '015'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/scenes',
                component: ComponentCreator('/docs/editor/scenes', '85b'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/scripts',
                component: ComponentCreator('/docs/editor/scripts', 'f38'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/shaders',
                component: ComponentCreator('/docs/editor/shaders', 'f1a'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/editor/shortcuts',
                component: ComponentCreator('/docs/editor/shortcuts', '780'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/engine-concepts/assets-folder',
                component: ComponentCreator('/docs/engine-concepts/assets-folder', '75c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/engine-concepts/collisions',
                component: ComponentCreator('/docs/engine-concepts/collisions', 'cdc'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/engine-concepts/components',
                component: ComponentCreator('/docs/engine-concepts/components', '637'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/engine-concepts/global-illumination',
                component: ComponentCreator('/docs/engine-concepts/global-illumination', '28b'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/engine-concepts/script-controls',
                component: ComponentCreator('/docs/engine-concepts/script-controls', 'c4f'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/getting-started/download-and-install',
                component: ComponentCreator('/docs/getting-started/download-and-install', '6e8'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/getting-started/first-project',
                component: ComponentCreator('/docs/getting-started/first-project', '9aa'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/getting-started/intro',
                component: ComponentCreator('/docs/getting-started/intro', 'a4c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/2d-objects',
                component: ComponentCreator('/docs/scripting-api/2d-objects', '62c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/3d-objects',
                component: ComponentCreator('/docs/scripting-api/3d-objects', 'e87'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/basic-object-types',
                component: ComponentCreator('/docs/scripting-api/basic-object-types', '5f8'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/camera-object',
                component: ComponentCreator('/docs/scripting-api/camera-object', 'ee3'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/collision-system',
                component: ComponentCreator('/docs/scripting-api/collision-system', 'e77'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/components',
                component: ComponentCreator('/docs/scripting-api/components', '6d9'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/grid-and-octree',
                component: ComponentCreator('/docs/scripting-api/grid-and-octree', '086'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/index',
                component: ComponentCreator('/docs/scripting-api/index', '00c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/input-system',
                component: ComponentCreator('/docs/scripting-api/input-system', '784'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/lighting-system',
                component: ComponentCreator('/docs/scripting-api/lighting-system', '13c'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/particle-emitter',
                component: ComponentCreator('/docs/scripting-api/particle-emitter', 'c36'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/shaders-system',
                component: ComponentCreator('/docs/scripting-api/shaders-system', 'faf'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/sound-system',
                component: ComponentCreator('/docs/scripting-api/sound-system', 'e83'),
                exact: true,
                sidebar: "tutorialSidebar"
              },
              {
                path: '/docs/scripting-api/text-system',
                component: ComponentCreator('/docs/scripting-api/text-system', '03a'),
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
    path: '/',
    component: ComponentCreator('/', '2e1'),
    exact: true
  },
  {
    path: '*',
    component: ComponentCreator('*'),
  },
];
