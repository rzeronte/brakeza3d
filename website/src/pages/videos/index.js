import React from 'react';
import Head from '@docusaurus/Head';
import Link from '@docusaurus/Link';
import Layout from '@theme/Layout';
import {videos, SITE_URL, videoUrl, thumbnailUrl} from '../../data/videos';

export default function Videos() {
    const jsonLd = {
        '@context': 'https://schema.org',
        '@type': 'ItemList',
        itemListElement: videos.map((v, i) => ({
            '@type': 'ListItem',
            position: i + 1,
            url: videoUrl(v),
            name: v.name,
        })),
    };

    return (
        <Layout
            title="Videos"
            description="Brakeza3D videos: a full tour of the open-source game engine demos and the Candelabra game preview."
        >
            <Head>
                <link rel="canonical" href={`${SITE_URL}/videos`} />
                <script type="application/ld+json">{JSON.stringify(jsonLd)}</script>
            </Head>
            <main>
                <div className="container" style={{paddingTop: '3rem', paddingBottom: '3rem'}}>
                    <h1>Videos</h1>
                    <p>Demos of the Brakeza3D open-source game engine and of the games made with it.</p>
                    <div className="row" style={{marginTop: '2rem'}}>
                        {videos.map((v) => (
                            <div key={v.id} className="col col--6" style={{marginBottom: '2rem'}}>
                                <div className="card" style={{height: '100%'}}>
                                    <div className="card__image">
                                        <Link to={`/videos/${v.slug}`}>
                                            <img
                                                src={thumbnailUrl(v)}
                                                alt={v.name}
                                                loading="lazy"
                                                style={{width: '100%', aspectRatio: '16 / 9', objectFit: 'cover'}}
                                            />
                                        </Link>
                                    </div>
                                    <div className="card__body">
                                        <h3><Link to={`/videos/${v.slug}`}>{v.name}</Link></h3>
                                        <p>{v.description}</p>
                                        <small>{v.durationLabel} · {v.uploadDate.slice(0, 10)}</small>
                                    </div>
                                </div>
                            </div>
                        ))}
                    </div>
                </div>
            </main>
        </Layout>
    );
}
