import React from 'react';
import Head from '@docusaurus/Head';
import Link from '@docusaurus/Link';
import Layout from '@theme/Layout';
import {videos, SITE_URL, videoUrl, embedUrl, watchOnYoutubeUrl, thumbnailUrl} from '../data/videos';

// Página de visualización de un vídeo: el vídeo es el contenido principal de la página (lo que Google
// exige para indexarlo), con título, descripción y datos estructurados VideoObject.
export default function VideoWatchPage({video}) {
    const others = videos.filter((v) => v.id !== video.id);

    const jsonLd = {
        '@context': 'https://schema.org',
        '@type': 'VideoObject',
        name: video.name,
        description: video.description,
        thumbnailUrl: [thumbnailUrl(video)],
        uploadDate: video.uploadDate,
        duration: video.duration,
        embedUrl: embedUrl(video),
        contentUrl: watchOnYoutubeUrl(video),
        url: videoUrl(video),
        publisher: {
            '@type': 'Organization',
            name: 'Brakeza3D',
            url: SITE_URL,
            logo: {'@type': 'ImageObject', url: `${SITE_URL}/img/logo.png`},
        },
    };

    return (
        <Layout title={video.name} description={video.description}>
            <Head>
                <link rel="canonical" href={videoUrl(video)} />
                <meta property="og:type" content="video.other" />
                <meta property="og:title" content={video.name} />
                <meta property="og:description" content={video.description} />
                <meta property="og:image" content={thumbnailUrl(video)} />
                <meta property="og:video" content={embedUrl(video)} />
                <script type="application/ld+json">{JSON.stringify(jsonLd)}</script>
            </Head>
            <main>
                <div className="container" style={{paddingTop: '2rem', paddingBottom: '3rem'}}>
                    <div className="row">
                        <div className="col col--10 col--offset-1">
                            <nav aria-label="breadcrumb" style={{marginBottom: '1rem'}}>
                                <Link to="/videos">Videos</Link> › {video.shortName}
                            </nav>

                            <h1>{video.name}</h1>

                            <div
                                style={{
                                    width: '100%',
                                    aspectRatio: '16 / 9',
                                    overflow: 'hidden',
                                    borderRadius: '12px',
                                    boxShadow: '0 4px 20px rgba(0, 0, 0, 0.15)',
                                    margin: '1.5rem 0',
                                }}
                            >
                                <iframe
                                    src={embedUrl(video)}
                                    title={video.name}
                                    style={{width: '100%', height: '100%', border: 0}}
                                    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
                                    allowFullScreen
                                />
                            </div>

                            <p style={{color: 'var(--ifm-color-emphasis-700)'}}>
                                {video.durationLabel} · Published{' '}
                                <time dateTime={video.uploadDate}>{video.uploadDate.slice(0, 10)}</time> ·{' '}
                                <a href={watchOnYoutubeUrl(video)} target="_blank" rel="noopener noreferrer">
                                    Watch on YouTube
                                </a>
                            </p>

                            <p style={{fontSize: '1.1rem'}}>{video.description}</p>

                            <h2>About Brakeza3D</h2>
                            <p>
                                Brakeza3D is a free, open-source 3D/2D game engine written in C++. Read the{' '}
                                <Link to="/docs/getting-started/intro">documentation</Link>, get it from the{' '}
                                <Link to="/downloads">downloads page</Link>, or browse the{' '}
                                <Link to="/gallery">gallery</Link> and the <Link to="/blog">blog</Link>.
                            </p>

                            {others.length > 0 && (
                                <>
                                    <h2>More videos</h2>
                                    <ul>
                                        {others.map((v) => (
                                            <li key={v.id}>
                                                <Link to={`/videos/${v.slug}`}>{v.name}</Link> ({v.durationLabel})
                                            </li>
                                        ))}
                                    </ul>
                                </>
                            )}
                        </div>
                    </div>
                </div>
            </main>
        </Layout>
    );
}
