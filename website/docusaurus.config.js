// @ts-check
// `@type` JSDoc annotations allow 2-editor autocompletion and type checking
// (when paired with `@ts-check`).
// There are various equivalent ways to declare your Docusaurus config.
// See: https://docusaurus.io/docs/api/docusaurus-config

import {themes as prismThemes} from 'prism-react-renderer';

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'Brakeza3D Game Engine',
  tagline: 'Simple and powerful cross-platform 2D/3D game engine',
  favicon: 'img/favicon.ico',

  // Set the production url of your site here
  url: 'https://brakeza.com',
  // Set the /<baseUrl>/ pathname under which your site is served
  // For GitHub pages deployment, it is often '/<projectName>/'
  baseUrl: '/',

  // GitHub pages deployment config.
  // If you aren't using GitHub pages, you don't need these.
  organizationName: 'rzeronte', // Usually your GitHub org/user name.
  projectName: 'brakeza3d', // Usually your repo name.

  onBrokenLinks: 'warn',
  onBrokenMarkdownLinks: 'warn',

  // Even if you don't use internationalization, you can use this field to set
  // useful metadata like html lang. For example, if your site is Chinese, you
  // may want to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        docs: {
          sidebarPath: './sidebars.js',
          // Please change this to your repo.
          // Remove this to remove the "edit this page" links.
          // editUrl: 'https://github.com/your-org/your-repo/tree/main/',
        },
        blog: {
          showReadingTime: true,
          feedOptions: {
            type: ['rss', 'atom'],
            xslt: true,
          },
          // Please change this to your repo.
          // Remove this to remove the "edit this page" links.
          // editUrl: 'https://github.com/your-org/your-repo/tree/main/',
          // Useful options to enforce blogging best practices
          onInlineTags: 'warn',
          onInlineAuthors: 'warn',
          onUntruncatedBlogPosts: 'warn',
        },
        theme: {
          customCss: './src/css/custom.css',
        },
        gtag: {
          trackingID: 'G-WYRLFVTYQN',
          anonymizeIP: true,
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      // Replace with your project's social card
      image: 'img/docusaurus-social-card.jpg',
      navbar: {
        title: 'Brakeza3D',
        logo: {
          alt: 'Brakeza3D Logo',
          src: 'img/logo.png',
        },
        items: [
          {
            type: 'docSidebar',
            sidebarId: 'tutorialSidebar',
            position: 'left',
            label: 'Documentation',
          },
          {to: '/downloads', label: 'Downloads', position: 'left'},
          {to: '/gallery', label: 'Gallery', position: 'left'},
          {to: '/blog', label: 'Blog', position: 'left'},
          {to: '/assets-hub', label: 'Assets Hub', position: 'left'},
          {to: '/contact', label: 'Contact', position: 'left'},
          {
            href: 'https://github.com/rzeronte/brakeza3d',
            label: 'GitHub',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Docs',
            items: [
              {
                label: 'Getting started',
                to: '/docs/getting-started/intro',
              },
              {
                label: 'Editor manual',
                to: '/docs/editor/overview',
              },
              {
                label: 'Engine concepts',
                to: '/docs/engine-concepts/script-controls',
              },
              {
                label: 'Scripting API',
                to: '/docs/scripting-api/index',
              },
            ],
          },
          {
            title: 'Community',
            items: [
              {
                label: 'Twitter',
                href: 'https://twitter.com/brakeza3d',
              },
              {
                label: 'Youtube',
                href: 'https://youtube.com/@brakeza3d',
              },
              {
                label: 'Linkedin',
                href: 'https://www.linkedin.com/in/eduardo-rodr%C3%ADguez-1796b8172/',
              },
            ],
          },
          {
            title: 'More',
            items: [
              {
                label: 'Blog',
                to: '/blog',
              },
              {
                label: 'GitHub',
                href: 'https://github.com/rzeronte/brakeza3d',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} Brakeza3D Game Engine - By Eduardo Rodríguez Álvarez`,
      },
      prism: {
        theme: prismThemes.github,
        darkTheme: prismThemes.dracula,
        additionalLanguages: ['lua'],
      },
    }),

};

export default config;
