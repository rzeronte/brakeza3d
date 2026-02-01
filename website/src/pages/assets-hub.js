import React from 'react';
import Link from '@docusaurus/Link';
import Layout from '@theme/Layout';
import styles from './assets-hub.module.css';

export default function AssetsHub() {
    return (
        <Layout
            title="Assets Hub"
            description="Share and discover community-made scripts and shaders compatible with Brakeza3D.">
            <main className={styles.mainContent}>
                <div className="container">
                    <h1>Scripts & Shaders Hub</h1>
                    <p className={styles.description}>
                        Share and discover community-made scripts and shaders compatible with Brakeza3D.
                        Enhance your projects with ready-to-use Lua scripts and GLSL shaders.
                    </p>

                    <div className={styles.buttons}>
                        <Link
                            className="button button--secondary button--lg"
                            href="https://hub.brakeza.com"
                            style={{color: 'white'}}>
                            🚀 Access the Hub
                        </Link>
                    </div>

                    <div className={styles.imagesSection}>
                        <img
                            src="/img/hub/01.png"
                            alt="Assets Hub Website"
                            className={styles.hubImage}
                        />
                    </div>
                </div>
            </main>
        </Layout>
    );
}
