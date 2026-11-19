// Videos de YouTube del sitio. Cada uno tiene su propia página de visualización (/videos/<slug>):
// Google solo indexa un vídeo si la página está dedicada a él (el vídeo es el contenido principal).
// Los datos (título, duración, fecha de subida) son los de YouTube; al añadir un vídeo nuevo, crear
// también src/pages/videos/<slug>.js que use <VideoWatchPage video={...} />.
export const videos = [
    {
        id: '_719nP1fJsY',
        slug: 'candelabra-preview',
        name: 'CANDELABRA — Preview | A Crime-City RTS made with Brakeza3D',
        shortName: 'CANDELABRA — Preview',
        description:
            'Preview of Candelabra, a crime-city real-time strategy game made with the open-source Brakeza3D engine. ' +
            'The Professor has been hung from a crane, in plain sight of the whole city.',
        duration: 'PT3M22S',
        durationLabel: '3:22',
        uploadDate: '2026-11-10T15:24:13-08:00',
        topics: ['Candelabra', 'real-time strategy', 'crime city', 'Brakeza3D game made with the engine'],
    },
    {
        id: 'VtCWW-WrppE',
        slug: 'engine-demo-tour',
        name: 'Brakeza3D v0.26.10 - Open Source 3D Game Engine: Full demo tour',
        shortName: 'Brakeza3D v0.26.10 — Full demo tour',
        description:
            'Brakeza3D is an open-source 3D/2D game engine written in C++ with OpenGL deferred rendering, an ImGui editor, ' +
            'Bullet physics and Lua scripting. This is a full tour of the engine demos.',
        duration: 'PT19M41S',
        durationLabel: '19:41',
        uploadDate: '2026-10-22T14:45:46-07:00',
        topics: ['open source game engine', 'C++', 'OpenGL', 'Bullet physics', 'Lua scripting', 'editor'],
    },
];

export const SITE_URL = 'https://brakeza.com';

export const videoUrl = (v) => `${SITE_URL}/videos/${v.slug}`;
export const embedUrl = (v) => `https://www.youtube.com/embed/${v.id}`;
export const watchOnYoutubeUrl = (v) => `https://www.youtube.com/watch?v=${v.id}`;
export const thumbnailUrl = (v) => `https://i.ytimg.com/vi/${v.id}/maxresdefault.jpg`;
