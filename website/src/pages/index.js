import React from 'react';
import clsx from 'clsx';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Layout from '@theme/Layout';
import styles from './index.module.css';
import useBaseUrl from '@docusaurus/useBaseUrl';

function HomepageHeader() {
    const {siteConfig} = useDocusaurusContext();
    return (
        <header className={clsx('hero hero--primary', styles.heroBanner)}>
            <div className="container">
                <h1 className="hero__title" style={{fontSize: '3.5rem'}}>
                    Brakeza3D v0.26.1 <br/>Released!
                </h1>
                <p className="hero__subtitle" style={{fontSize: '1.5rem'}}>
                    A powerful 2D/3D game engine for indie developers
                </p>
                <div className={styles.buttons} style={{marginTop: '2rem'}}>
                    <Link
                        className="button button--primary button--lg"
                        to="docs/getting-started/intro">
                        📖 Get started
                    </Link>
                    <Link
                        className="button button--primary button--lg"
                        to="/downloads"
                        style={{marginLeft: '1rem'}}>
                        ⬇️ Downloads
                    </Link>
                    <Link
                        className="button button--primary button--lg"
                        to="/gallery"
                        style={{marginLeft: '1rem'}}>
                        🎨 Show me more
                    </Link>
                </div>
            </div>
        </header>
    );
}

function Stats() {
    return (
        <section style={{padding: '3rem 0', backgroundColor: '#1c1e21', color: 'white'}}>
            <div className="container">
                <div className="row" style={{textAlign: 'center'}}>
                    <div className="col col--3">
                        <h2 style={{fontSize: '2.5rem', marginBottom: '0.5rem'}}>2D/3D</h2>
                        <p style={{fontSize: '1.1rem', color: '#aaa'}}>Dual Workflow</p>
                    </div>
                    <div className="col col--3">
                        <h2 style={{fontSize: '2.5rem', marginBottom: '0.5rem'}}>Extensible</h2>
                        <p style={{fontSize: '1.1rem', color: '#aaa'}}>Reusable LUA files + Customizable shaders GLSL/Nodes based </p>
                    </div>
                    <div className="col col--3">
                        <h2 style={{fontSize: '2.5rem', marginBottom: '0.5rem'}}>GUI</h2>
                        <p style={{fontSize: '1.1rem', color: '#aaa'}}>Intuitive drag & drop, code editor with syntax highlighting</p>
                    </div>
                    <div className="col col--3">
                        <h2 style={{fontSize: '2.5rem', marginBottom: '0.5rem'}}>Opensource</h2>
                        <p style={{fontSize: '1.1rem', color: '#aaa'}}>C++ / SDL / Bullet / OpenGL / GLM / Assimp / LUA</p>
                    </div>
                </div>
            </div>
        </section>
    );
}

const FeatureList = [
    {
        title: '🎮 Game-Ready',
        description: 'Built-in physics, collision detection, particle systems, and lighting. Everything you need to create professional games.',
    },
    {
        title: '📝 Lua Scripting',
        description: 'Write game logic in Lua with full access to the engine API. Fast iteration without recompilation.',
    },
    {
        title: '🎨 Powerful Editor',
        description: 'Integrated scene 2-editor with real-time preview. Drag-and-drop assets, visual debugging, and more.',
    },
    {
        title: '⚡ High Performance',
        description: 'Optimized rendering pipeline with modern OpenGL. GLSL shaders for custom visual effects.',
    },
    {
        title: '🔧 Cross-Platform',
        description: 'Build once, deploy to multiple platforms. Windows, Linux, and more with consistent behavior.',
    },
    {
        title: '📚 Well Documented',
        description: 'Comprehensive documentation, tutorials, and examples to get you started quickly.',
    },
];

function Feature({title, description}) {
    return (
        <div className={clsx('col col--4')} style={{marginBottom: '2rem'}}>
            <div className="card" style={{height: '100%'}}>
                <div className="card__body">
                    <h3>{title}</h3>
                    <p>{description}</p>
                </div>
            </div>
        </div>
    );
}

function HomepageFeatures() {
    return (
        <section style={{padding: '4rem 0'}}>
            <div className="container">
                <div className="row">
                    {FeatureList.map((props, idx) => (
                        <Feature key={idx} {...props} />
                    ))}
                </div>
            </div>
        </section>
    );
}

function ProjectShowcase() {
    const projects = [
        {image: 'img/gallery/01.png', title: 'Show case 1'},
        {image: 'img/gallery/02.png', title: 'Show case 2'},
        {image: 'img/gallery/03.png', title: 'Show case 3'},
        {image: 'img/gallery/04.png', title: 'Show case 4'},
        {image: 'img/gallery/05.png', title: 'Show case 5'},
        {image: 'img/gallery/06.png', title: 'Show case 6'},
        {image: 'img/gallery/07.png', title: 'Show case 7'},
        {image: 'img/gallery/08.png', title: 'Show case 8'},
        {image: 'img/gallery/09.png', title: 'Show case 9'},
    ];

    return (
        <section style={{padding: '4rem 0', backgroundColor: '#f5f5f5'}}>
            <div className="container">
                <div className="row">
                    {projects.map((project, idx) => (
                        <div key={idx} className="col col--4">
                            <div className="card">
                                <div className="card__image">
                                    <img
                                        src={useBaseUrl(project.image)}
                                        alt={project.title}
                                        style={{width: '100%'}}
                                    />
                                </div>
                                <div className="card__body">
                                    <h4>{project.title}</h4>
                                </div>
                            </div>
                        </div>
                    ))}
                </div>

                <div style={{textAlign: 'center', marginTop: '2rem'}}>
                    <Link className="button button--primary button--lg" to="/gallery">
                        View Full Gallery →
                    </Link>
                </div>
            </div>
        </section>
    );
}

function FinalCTA() {
    return (
        <section style={{
            padding: '4rem 0',
            background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
            color: 'white',
            textAlign: 'center'
        }}>
            <div className="container">
                <h2 style={{fontSize: '2.5rem', marginBottom: '1rem'}}>
                    Ready to Start Creating?
                </h2>
                <p style={{fontSize: '1.3rem', marginBottom: '2rem', opacity: 0.9}}>
                    Join developers worldwide building amazing games with Brakeza3D
                </p>
                <div>
                    <Link
                        className="button button--primary button--lg"
                        to="/docs/getting-started/intro">
                        📖 Get started
                    </Link>
                    <Link
                        className="button button--primary button--lg"
                        to="/downloads"
                        style={{marginLeft: '1rem'}}>
                        ⬇️ Downloads
                    </Link>
                    <Link
                        className="button button--primary button--lg"
                        to="/gallery"
                        style={{marginLeft: '1rem'}}>
                        🎨 Show me more
                    </Link>

                </div>
            </div>
        </section>
    );
}

export default function Home() {
    const {siteConfig} = useDocusaurusContext();
    return (
        <Layout
            title={`Home`}
            description="Build amazing 2D and 3D games with Brakeza3D Engine">
            <HomepageHeader />
            <main>
                <Stats />
                <ProjectShowcase />
                <HomepageFeatures />
            </main>
            <FinalCTA />
        </Layout>
    );
}
