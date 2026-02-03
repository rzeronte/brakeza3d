// scripts/generate-doc-tree.js
const fs = require('fs');
const path = require('path');
const matter = require('gray-matter');

const sidebars = require('../sidebars.js');
const docsPath = path.join(__dirname, '../docs');

// Leer la URL base desde docusaurus.config.js
let baseUrl = 'https://brakeza.com'
//let baseUrl = 'http://localhost:3000/brakeza3d';

try {
    const docusaurusConfig = require('../docusaurus.config.js');
    if (docusaurusConfig.url) {
        baseUrl = docusaurusConfig.url;
        if (docusaurusConfig.baseUrl && docusaurusConfig.baseUrl !== '/') {
            baseUrl += docusaurusConfig.baseUrl.replace(/\/$/, '');
        }
    }
    console.log(`🌐 Using base URL: ${baseUrl}`);
} catch (e) {
    console.warn('⚠️  Could not read docusaurus.config.js, using default URL');
}

if (process.env.DOCS_BASE_URL) {
    baseUrl = process.env.DOCS_BASE_URL;
    console.log(`🌐 Using base URL from environment: ${baseUrl}`);
}

// Asegurar que la URL termina con /
function ensureTrailingSlash(url) {
    if (!url || url === "") return "";
    return url.endsWith('/') ? url : url + '/';
}

// Normalizar path para URL (convertir \ a /)
function normalizePathForUrl(pathStr) {
    return pathStr.replace(/\\/g, '/');
}

// Remover prefijos numéricos de Docusaurus (ej: "1-intro" -> "intro")
function removeNumberPrefix(str) {
    return str.replace(/^\d+[-_]/, '');
}

// Extraer prefijo numérico de Docusaurus (ej: "1-intro" -> 1, "intro" -> null)
function extractNumberPrefix(str) {
    const match = str.match(/^(\d+)[-_]/);
    return match ? parseInt(match[1], 10) : null;
}

// Formatear nombre de archivo a título legible
function formatFileName(filename) {
    // Remover prefijo numérico
    let clean = removeNumberPrefix(filename);
    // Reemplazar guiones y guiones bajos con espacios
    clean = clean.replace(/[-_]/g, ' ');
    // Capitalizar primera letra de cada palabra
    clean = clean.split(' ').map(word =>
        word.charAt(0).toUpperCase() + word.slice(1).toLowerCase()
    ).join(' ');
    return clean;
}

// Leer frontmatter de un archivo markdown
function getDocMetadata(filePath) {
    try {
        const content = fs.readFileSync(filePath, 'utf8');
        const { data } = matter(content);
        const basename = path.basename(filePath, path.extname(filePath));
        const formattedName = formatFileName(basename);

        // Usar sidebar_position del frontmatter, o el prefijo numérico del nombre, o 999
        const prefixPosition = extractNumberPrefix(basename);
        const position = data.sidebar_position ?? prefixPosition ?? 999;

        return {
            title: data.title || data.sidebar_label || formattedName,
            sidebarLabel: data.sidebar_label || data.title || formattedName,
            sidebarPosition: position,
            slug: data.slug,
        };
    } catch (e) {
        const basename = path.basename(filePath, path.extname(filePath));
        const formattedName = formatFileName(basename);
        const prefixPosition = extractNumberPrefix(basename);

        return {
            title: formattedName,
            sidebarLabel: formattedName,
            sidebarPosition: prefixPosition ?? 999,
            slug: null,
        };
    }
}

// Construir URL de Docusaurus (elimina prefijos numéricos)
function buildDocusaurusUrl(relativePath) {
    let cleanPath = normalizePathForUrl(relativePath);
    const segments = cleanPath.split('/');
    const cleanSegments = segments.map(segment => removeNumberPrefix(segment));
    return cleanSegments.join('/');
}

// Construir URL absoluta con barra final
function makeAbsoluteUrl(relativePath, customSlug = null) {
    if (!relativePath || relativePath === "") return "";

    if (relativePath.startsWith('http://') || relativePath.startsWith('https://')) {
        return ensureTrailingSlash(relativePath);
    }

    if (customSlug) {
        let url;
        if (customSlug.startsWith('/')) {
            // Slug absoluto (ej: slug: /my-page) - va directo desde la raíz
            url = baseUrl + customSlug;
        } else {
            // Slug relativo (ej: slug: 2d-objects)
            // Mantener la estructura de carpetas padre
            const docusaurusPath = buildDocusaurusUrl(relativePath);
            const pathParts = docusaurusPath.split('/');

            // Reemplazar el último segmento (nombre del archivo) con el slug
            pathParts[pathParts.length - 1] = customSlug;

            const finalPath = pathParts.join('/');

            // Evitar doble barra
            if (finalPath.startsWith('/')) {
                url = baseUrl + finalPath;
            } else {
                url = baseUrl + '/' + finalPath;
            }
        }
        return ensureTrailingSlash(url);
    }

    const docusaurusPath = buildDocusaurusUrl(relativePath);
    const finalUrl = docusaurusPath.startsWith('/')
        ? baseUrl + docusaurusPath
        : baseUrl + '/' + docusaurusPath;

    return ensureTrailingSlash(finalUrl);
}

// Leer un directorio autogenerado
function readAutogeneratedDir(dirName) {
    const dirPath = path.join(docsPath, dirName);

    if (!fs.existsSync(dirPath)) {
        console.warn(`⚠️  Directory not found: ${dirPath}`);
        return [];
    }

    const items = [];
    const entries = fs.readdirSync(dirPath, { withFileTypes: true });

    for (const entry of entries) {
        const fullPath = path.join(dirPath, entry.name);
        const relativePath = normalizePathForUrl(path.join(dirName, entry.name));

        if (entry.isDirectory()) {
            const categoryJsonPath = path.join(fullPath, '_category_.json');
            let categoryLabel = entry.name;
            // Usar prefijo numérico del nombre del directorio como fallback
            const prefixPosition = extractNumberPrefix(entry.name);
            let categoryPosition = prefixPosition ?? 999;

            if (fs.existsSync(categoryJsonPath)) {
                try {
                    const categoryData = JSON.parse(fs.readFileSync(categoryJsonPath, 'utf8'));
                    categoryLabel = categoryData.label || formatFileName(entry.name);
                    categoryPosition = categoryData.position ?? prefixPosition ?? 999;
                } catch (e) {
                    console.warn(`⚠️  Error reading _category_.json in ${entry.name}`);
                }
            } else {
                categoryLabel = formatFileName(entry.name);
            }

            items.push({
                name: categoryLabel,
                url: "",
                position: categoryPosition,
                children: readAutogeneratedDir(relativePath)
            });
        } else if (entry.name.endsWith('.md') || entry.name.endsWith('.mdx')) {
            const metadata = getDocMetadata(fullPath);
            const docPath = relativePath.replace(/\.(md|mdx)$/, '');

            items.push({
                name: metadata.sidebarLabel || metadata.title,
                url: makeAbsoluteUrl(`/docs/${docPath}`, metadata.slug),
                position: metadata.sidebarPosition || 999,
                children: []
            });
        }
    }

    items.sort((a, b) => a.position - b.position);

    return items.map(({ position, ...rest }) => rest);
}

// Procesar un item del sidebar
function processItem(item) {
    if (typeof item === 'string') {
        const docPath = path.join(docsPath, `${item}.md`);
        const docPathMdx = path.join(docsPath, `${item}.mdx`);

        let metadata = { title: formatFileName(item), slug: null };
        if (fs.existsSync(docPath)) {
            metadata = getDocMetadata(docPath);
        } else if (fs.existsSync(docPathMdx)) {
            metadata = getDocMetadata(docPathMdx);
        }

        const normalizedItem = normalizePathForUrl(item);

        return {
            name: metadata.sidebarLabel || metadata.title,
            url: makeAbsoluteUrl(`/docs/${normalizedItem}`, metadata.slug),
            children: []
        };
    } else if (item.type === 'category') {
        const children = [];

        for (const subItem of item.items || []) {
            if (subItem.type === 'autogenerated') {
                const autogenerated = readAutogeneratedDir(subItem.dirName);
                children.push(...autogenerated);
            } else {
                const processed = processItem(subItem);
                if (processed) children.push(processed);
            }
        }

        return {
            name: item.label,
            url: "",
            children: children
        };
    } else if (item.type === 'doc') {
        const normalizedId = normalizePathForUrl(item.id);

        let metadata = { slug: null };
        const docPath = path.join(docsPath, `${item.id}.md`);
        const docPathMdx = path.join(docsPath, `${item.id}.mdx`);

        if (fs.existsSync(docPath)) {
            metadata = getDocMetadata(docPath);
        } else if (fs.existsSync(docPathMdx)) {
            metadata = getDocMetadata(docPathMdx);
        }

        return {
            name: item.label || item.id,
            url: makeAbsoluteUrl(`/docs/${normalizedId}`, metadata.slug),
            children: []
        };
    } else if (item.type === 'link') {
        return {
            name: item.label,
            url: ensureTrailingSlash(item.href),
            children: []
        };
    }

    return null;
}

// Convertir sidebar a árbol
function convertSidebarToTree(sidebars) {
    const result = { root: [] };

    if (sidebars.default && Array.isArray(sidebars.default.tutorialSidebar)) {
        sidebars.default.tutorialSidebar.forEach(item => {
            const node = processItem(item);
            if (node) result.root.push(node);
        });
    } else if (sidebars.tutorialSidebar && Array.isArray(sidebars.tutorialSidebar)) {
        sidebars.tutorialSidebar.forEach(item => {
            const node = processItem(item);
            if (node) result.root.push(node);
        });
    }

    return result;
}

// Ejecutar
try {
    console.log('📚 Generating documentation tree...');
    const tree = convertSidebarToTree(sidebars);

    const outputPath = path.join(__dirname, '../static/doc-tree.json');
    const staticDir = path.join(__dirname, '../static');

    if (!fs.existsSync(staticDir)) {
        fs.mkdirSync(staticDir, { recursive: true });
    }

    fs.writeFileSync(outputPath, JSON.stringify(tree, null, 2));

    console.log('✅ Documentation tree generated successfully!');
    console.log(`📄 Location: static/doc-tree.json`);
    console.log(`📊 Total categories: ${tree.root.length}`);

    function countDocs(node) {
        let count = node.url && node.url !== "" ? 1 : 0;
        if (node.children) {
            count += node.children.reduce((sum, child) => sum + countDocs(child), 0);
        }
        return count;
    }
    const totalDocs = tree.root.reduce((sum, node) => sum + countDocs(node), 0);
    console.log(`📖 Total documents: ${totalDocs}`);

    if (tree.root.length > 0 && tree.root[0].children && tree.root[0].children.length > 0) {
        const firstDoc = tree.root[0].children[0];
        if (firstDoc.url) {
            console.log(`🔗 Example URL: ${firstDoc.url}`);
        }
    }

} catch (error) {
    console.error('❌ Error generating tree:', error.message);
    console.error(error.stack);
    process.exit(1);
}