import React from 'react';
import Layout from '@theme/Layout';
import useBaseUrl from '@docusaurus/useBaseUrl';

const galleryItems = [
  {
    title: 'Screenshot #01',
    description: 'Game demo built with Brakeza3D Game Engine',
    image: '/img/gallery/01.png',
  },
  {
    title: 'Screenshot #02',
    description: '3D scene showcasing lighting effects',
    image: '/img/gallery/02.png',
  },
  {
    title: 'Screenshot #03',
    description: 'Physics simulation demo',
    image: '/img/gallery/03.png',
  },
  {
    title: 'Screenshot #04',
    description: 'Particle system effects',
    image: '/img/gallery/04.png',
  },
  {
    title: 'Screenshot #05',
    description: 'Advanced rendering techniques',
    image: '/img/gallery/05.png',
  },
  {
    title: 'Screenshot #06',
    description: 'Game prototype',
    image: '/img/gallery/06.png',
  },
  {
    title: 'Screenshot #07',
    description: 'Shader effects demo',
    image: '/img/gallery/07.png',
  },
  {
    title: 'Screenshot #08',
    description: '3D model showcase',
    image: '/img/gallery/08.png',
  },
  {
    title: 'Screenshot #09',
    description: 'Interactive scene',
    image: '/img/gallery/09.png',
  },
  {
    title: 'Screenshot #10',
    description: 'User interface implementation',
    image: '/img/gallery/10.png',
  },
  {
    title: 'Screenshot #11',
    description: 'Menu system example',
    image: '/img/gallery/11.png',
  },
  {
    title: 'Screenshot #12',
    description: 'HUD elements showcase',
    image: '/img/gallery/12.png',
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
