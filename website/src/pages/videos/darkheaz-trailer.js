import React from 'react';
import VideoWatchPage from '../../components/VideoWatchPage';
import {videos} from '../../data/videos';

export default function DarkheazTrailer() {
    return <VideoWatchPage video={videos.find((v) => v.slug === 'darkheaz-trailer')} />;
}
