// scripts/validate-doc-urls.js
const fs = require('fs');
const path = require('path');
const https = require('https');
const http = require('http');

// Configuración
const DOC_TREE_PATH = path.join(__dirname, '../static/doc-tree.json');
const CONCURRENT_REQUESTS = 5; // Peticiones simultáneas
const TIMEOUT = 10000; // 10 segundos timeout

// Colores para consola
const colors = {
    reset: '\x1b[0m',
    green: '\x1b[32m',
    red: '\x1b[31m',
    yellow: '\x1b[33m',
    blue: '\x1b[34m',
    gray: '\x1b[90m',
};

// Función para hacer petición HTTP
function checkUrl(url) {
    return new Promise((resolve) => {
        const protocol = url.startsWith('https') ? https : http;

        const request = protocol.get(url, { timeout: TIMEOUT }, (res) => {
            resolve({
                url: url,
                status: res.statusCode,
                ok: res.statusCode === 200,
                redirected: res.statusCode >= 300 && res.statusCode < 400,
            });
        });

        request.on('timeout', () => {
            request.destroy();
            resolve({
                url: url,
                status: 0,
                ok: false,
                error: 'Timeout',
            });
        });

        request.on('error', (err) => {
            resolve({
                url: url,
                status: 0,
                ok: false,
                error: err.message,
            });
        });
    });
}

// Extraer todas las URLs del árbol
function extractUrls(node, urls = []) {
    if (node.url && node.url !== "") {
        urls.push(node.url);
    }

    if (node.children && Array.isArray(node.children)) {
        node.children.forEach(child => extractUrls(child, urls));
    }

    return urls;
}

// Dividir array en chunks para procesar en lotes
function chunkArray(array, size) {
    const chunks = [];
    for (let i = 0; i < array.length; i += size) {
        chunks.push(array.slice(i, i + size));
    }
    return chunks;
}

// Validar URLs en lotes
async function validateUrls(urls) {
    const results = [];
    const chunks = chunkArray(urls, CONCURRENT_REQUESTS);

    console.log(`${colors.blue}🔍 Validating ${urls.length} URLs...${colors.reset}\n`);

    let processed = 0;
    for (const chunk of chunks) {
        const chunkResults = await Promise.all(chunk.map(checkUrl));
        results.push(...chunkResults);

        processed += chunk.length;
        process.stdout.write(`${colors.gray}Progress: ${processed}/${urls.length}${colors.reset}\r`);
    }

    console.log('\n');
    return results;
}

// Generar reporte
function generateReport(results) {
    const success = results.filter(r => r.ok);
    const failed = results.filter(r => !r.ok && !r.redirected);
    const redirected = results.filter(r => r.redirected);

    console.log('='.repeat(80));
    console.log(`${colors.blue}📊 VALIDATION REPORT${colors.reset}`);
    console.log('='.repeat(80));
    console.log();

    // Resumen
    console.log(`${colors.green}✅ Success: ${success.length}${colors.reset}`);
    console.log(`${colors.red}❌ Failed: ${failed.length}${colors.reset}`);
    console.log(`${colors.yellow}↗️  Redirected: ${redirected.length}${colors.reset}`);
    console.log(`📝 Total: ${results.length}`);
    console.log();

    // URLs fallidas
    if (failed.length > 0) {
        console.log(`${colors.red}❌ FAILED URLS:${colors.reset}`);
        console.log('-'.repeat(80));
        failed.forEach(result => {
            console.log(`${colors.red}✗${colors.reset} ${result.url}`);
            if (result.error) {
                console.log(`  ${colors.gray}Error: ${result.error}${colors.reset}`);
            } else {
                console.log(`  ${colors.gray}Status: ${result.status}${colors.reset}`);
            }
        });
        console.log();
    }

    // URLs redirigidas (advertencia)
    if (redirected.length > 0) {
        console.log(`${colors.yellow}↗️  REDIRECTED URLS (should have trailing slash):${colors.reset}`);
        console.log('-'.repeat(80));
        redirected.forEach(result => {
            console.log(`${colors.yellow}⚠${colors.reset} ${result.url}`);
            console.log(`  ${colors.gray}Status: ${result.status}${colors.reset}`);
        });
        console.log();
    }

    // Resultado final
    console.log('='.repeat(80));
    if (failed.length === 0 && redirected.length === 0) {
        console.log(`${colors.green}✅ All URLs are valid!${colors.reset}`);
        return 0;
    } else if (failed.length > 0) {
        console.log(`${colors.red}❌ Validation failed: ${failed.length} broken URLs${colors.reset}`);
        return 1;
    } else {
        console.log(`${colors.yellow}⚠️  Warning: ${redirected.length} URLs redirect (missing trailing slash?)${colors.reset}`);
        return 1;
    }
}

// Main
async function main() {
    try {
        // Verificar que existe el archivo
        if (!fs.existsSync(DOC_TREE_PATH)) {
            console.error(`${colors.red}❌ Error: doc-tree.json not found at ${DOC_TREE_PATH}${colors.reset}`);
            console.error(`${colors.yellow}Run 'npm run generate-doc-tree' first${colors.reset}`);
            process.exit(1);
        }

        // Leer el árbol
        const treeJson = fs.readFileSync(DOC_TREE_PATH, 'utf8');
        const tree = JSON.parse(treeJson);

        // Extraer URLs
        const urls = [];
        if (tree.root && Array.isArray(tree.root)) {
            tree.root.forEach(node => extractUrls(node, urls));
        }

        // Eliminar duplicados
        const uniqueUrls = [...new Set(urls)];

        if (uniqueUrls.length === 0) {
            console.log(`${colors.yellow}⚠️  No URLs found to validate${colors.reset}`);
            process.exit(0);
        }

        // Validar URLs
        const results = await validateUrls(uniqueUrls);

        // Generar reporte
        const exitCode = generateReport(results);

        // Guardar reporte en JSON (opcional)
        const reportPath = path.join(__dirname, '../static/validation-report.json');
        fs.writeFileSync(reportPath, JSON.stringify({
            timestamp: new Date().toISOString(),
            total: results.length,
            success: results.filter(r => r.ok).length,
            failed: results.filter(r => !r.ok && !r.redirected).length,
            redirected: results.filter(r => r.redirected).length,
            results: results,
        }, null, 2));

        console.log(`\n${colors.gray}📄 Detailed report saved to: ${reportPath}${colors.reset}`);

        process.exit(exitCode);

    } catch (error) {
        console.error(`${colors.red}❌ Fatal error:${colors.reset}`, error.message);
        process.exit(1);
    }
}

main();