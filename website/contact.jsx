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
              <h1>Contact Us</h1>
              <p className="hero__subtitle">
                Have questions or feedback? We'd love to hear from you!
              </p>

              <div className="card" style={{marginTop: '2rem'}}>
                <div className="card__body">
                  <div style={{marginTop: '2rem'}}>
                    <h3>📧 Email</h3>
                    <p>
                      <a href="mailto:contact@brakeza3d.com">contact@brakeza3d.com</a>
                    </p>
                  </div>

                  <div style={{marginTop: '2rem'}}>
                    <h3>💬 Community</h3>
                    <p>
                      Join our Discord server: <a href="https://discord.gg/your-invite" target="_blank">discord.gg/brakeza3d</a>
                    </p>
                  </div>

                  <div style={{marginTop: '2rem'}}>
                    <h3>🐙 GitHub</h3>
                    <p>
                      Report issues or contribute: <a href="https://github.com/your-org/brakeza3d" target="_blank">github.com/your-org/brakeza3d</a>
                    </p>
                  </div>
                  <div style={{marginTop: '2rem'}}>
                    <h3>🐦 Social Media</h3>
                    <p>
                      Follow us on Twitter: <a href="https://twitter.com/your-handle" target="_blank">@brakeza3d</a>
                    </p>
                  </div>
                </div>
              </div>

              <div className="card" style={{marginTop: '2rem'}}>
                <div className="card__body">
                  <h2>Support</h2>
                  <p>
                    For technical support, please check our <a href="/docs/basics">documentation</a> first.
                    If you can't find what you're looking for, feel free to reach out via email or Discord.
                  </p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </main>
    </Layout>
  );
}
