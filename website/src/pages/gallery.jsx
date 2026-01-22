import React from 'react';
import Layout from '@theme/Layout';
import useBaseUrl from '@docusaurus/useBaseUrl';

const galleryItems = [
  {
    title: 'Project 1',
    description: 'Game demo built with Brakeza3D Game Engine',
    image: '/img/gallery/01.png',
  },
  {
    title: 'Project 2',
    description: '3D scene showcasing lighting effects',
    image: '/img/gallery/02.png',
  },
  {
    title: 'Project 3',
    description: 'Physics simulation demo',
    image: '/img/gallery/03.png',
  },
  {
    title: 'Project 4',
    description: 'Particle system effects',
    image: '/img/gallery/04.png',
  },
  {
    title: 'Project 5',
    description: 'Advanced rendering techniques',
    image: '/img/gallery/05.png',
  },
  {
    title: 'Project 6',
    description: 'Game prototype',
    image: '/img/gallery/06.png',
  },
  {
    title: 'Project 7',
    description: 'Shader effects demo',
    image: '/img/gallery/07.png',
  },
  {
    title: 'Project 8',
    description: '3D model showcase',
    image: '/img/gallery/08.png',
  },
  {
    title: 'Project 9',
    description: 'Interactive scene',
    image: '/img/gallery/09.png',
  },
  {
    title: 'GUI Demo 1',
    description: 'User interface implementation',
    image: '/img/gallery/gui_01.png',
  },
  {
    title: 'GUI Demo 2',
    description: 'Menu system example',
    image: '/img/gallery/gui_02.png',
  },
  {
    title: 'GUI Demo 3',
    description: 'HUD elements showcase',
    image: '/img/gallery/gui_03.png',
  },
  {
    title: 'GUI Demo 4',
    description: 'UI components library',
    image: '/img/gallery/gui_04.png',
  },
];

function GalleryItem({title, description, image}) {
  return (
    <div className="col col--4" style={{marginBottom: '2rem'}}>
      <div className="card">
        <div className="card__image">
          <img 
            src={useBaseUrl(image)}
            alt={title} 
            style={{width: '100%', height: '250px', objectFit: 'cover'}} 
          />
        </div>
        <div className="card__body">
          <h3>{title}</h3>
          <p>{description}</p>
        </div>
      </div>
    </div>
  );
}

export default function Gallery() {
  return (
    <Layout
      title="Gallery"
      description="Games and projects made with Brakeza3D Engine">
      <main>
        <div className="container" style={{paddingTop: '3rem', paddingBottom: '3rem'}}>
          <div className="row">
            <div className="col">
              <h1>Gallery</h1>
              <p className="hero__subtitle">
                Showcase of games and projects made with Brakeza3D Engine
              </p>
            </div>
          </div>
          <div className="row">
            {galleryItems.map((props, idx) => (
              <GalleryItem key={idx} {...props} />
            ))}
          </div>
        </div>
      </main>
    </Layout>
  );
}
