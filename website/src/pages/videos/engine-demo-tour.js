import React from 'react';
import VideoWatchPage from '../../components/VideoWatchPage';
import {videos} from '../../data/videos';

export default function EngineDemoTour() {
    return <VideoWatchPage video={videos.find((v) => v.slug === 'engine-demo-tour')} />;
}
