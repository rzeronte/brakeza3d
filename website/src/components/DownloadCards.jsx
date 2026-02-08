import React from 'react';
import styles from './DownloadCards.module.css';

const downloads = [
    {
        icon: '/img/logo_windows.png',
        title: 'Windows 10/11',
        arch: 'amd64',
        size: '68.9 MB',
        url: 'https://github.com/rzeronte/brakeza3d/releases/download/0.26.2/Brakeza3D-v0.26.2-amd64-Windows-installer.exe',
        label: 'Download .exe'
    },
    {
        icon: '/img/logo_linux.png',
        title: 'Ubuntu/Debian',
        arch: 'amd64',
        size: '33.3 MB',
        url: 'https://github.com/rzeronte/brakeza3d/releases/download/0.26.2/Brakeza3D-v0.26.2-amd64-Linux-installer.deb',
        label: 'Download .deb'
    },
    {
        icon: '/img/logo_github.png',
        title: 'Source Code',
        arch: 'Build it yourself',
        size: '44.7 MB',
        url: 'https://github.com/rzeronte/brakeza3d/archive/refs/heads/master.zip',
        label: 'Download .zip'
    }
];

export default function DownloadCards() {
    return (
        <div className={styles.grid}>
            {downloads.map((item, idx) => (
                <div key={idx} className={styles.card}>
                    <img src={item.icon} alt={item.title} className={styles.icon} />
                    <h3>{item.title}</h3>
                    <p className={styles.arch}>{item.arch}</p>
                    <a href={item.url} className={styles.btn}>
                        {item.label}
                    </a>
                    <span className={styles.size}>{item.size}</span>
                </div>
            ))}
        </div>
    );
}