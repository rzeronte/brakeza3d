import React from 'react';
import Layout from '@theme/Layout';

export default function Contact() {
  return (
    <Layout
      title="Contact"
      description="Get in touch with the Brakeza3D team">
      <main>
        <div className="container" style={{paddingTop: '3rem', paddingBottom: '3rem'}}>
          <div className="row">
            <div className="col col--8 col--offset-2">
              <h1>Contact</h1>

              {/* SECCIÓN DE CONTACTO PRINCIPAL */}
              <div className="card" style={{marginTop: '2rem'}}>
                <div className="card__body">
                  <h2>Get in Touch</h2>
                  
                  <div style={{marginTop: '2rem'}}>
                    <h3>📧 Email</h3>
                    <p>
                      General inquiries: <a href="mailto:eduardo@brakeza.com">Send me a email!</a><br/>
                    </p>
                  </div>

                  <div style={{marginTop: '2rem'}}>
                    <h3>🐙 GitHub</h3>
                    <p>
                      Report issues: <a href="https://github.com/rzeronte/brakeza3d/issues" target="_blank" rel="noopener noreferrer">GitHub Issues</a><br/>
                      Contribute: <a href="https://github.com/rzeronte/brakeza3d" target="_blank" rel="noopener noreferrer">GitHub Repository</a>
                    </p>
                  </div>

                  <div style={{marginTop: '2rem'}}>
                    <h3>🐦 Social Media</h3>
                    <p>
                      Twitter: <a href="https://twitter.com/brakeza3d" target="_blank" rel="noopener noreferrer">@brakeza3d</a><br/>
                      YouTube: <a href="https://youtube.com/@brakeza3d" target="_blank" rel="noopener noreferrer">Brakeza3D Channel</a><br/>
                      LinkedIn: <a href="https://www.linkedin.com/in/eduardo-rodr%C3%ADguez-1796b8172/" target="_blank" rel="noopener noreferrer">Eduardo Rodríguez Álvarez</a>
                    </p>
                  </div>
                </div>
              </div>

            </div>
          </div>
        </div>
      </main>
    </Layout>
  );
}
