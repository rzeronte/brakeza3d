
#include "../../headers/Game/NPCEnemyBody.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Game/Projectile3DBody.h"
#include "../../headers/Render/Billboard.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/EngineBuffers.h"

NPCEnemyBody::NPCEnemyBody() : state(EnemyState::ENEMY_STATE_STOP), stepIA(EngineSetup::getInstance()->TIME_STEP_IA_ENEMIES)
{
    this->counterIA = new Counter();
    this->counterIA->setStep( stepIA );

    this->counterInjuried = new Counter();
    this->counterInjuried->setStep( 0.25 );

    this->projectileTemplate = new SpriteDirectional3D();
    projectileTemplate->addAnimationDirectional2D(EngineSetup::getInstance()->SPRITES_FOLDER + "bullet/idle", 1, 20, false, -1);
    projectileTemplate->setAnimation(0);
    projectileTemplate->width  = 0.5;
    projectileTemplate->height = 0.5;
}

void NPCEnemyBody::evalStatusMachine(bool raycastResult, float raycastlength, Camera3D *cam, btDiscreteDynamicsWorld *dynamicsWorld, std::vector<Object3D*> &gameObjects)
{
    switch(state) {
        case EnemyState::ENEMY_STATE_ATTACK:
            this->syncPathFindingRotation();

            if  (this->counterCadence->isFinished() ) {
                this->shoot(cam, dynamicsWorld, gameObjects);
            }

            if ( raycastlength >= this->getRange() ) {
                this->setAnimation( EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_WALK );
                this->state = EnemyState::ENEMY_STATE_STOP;
            }
            break;
        case EnemyState::ENEMY_STATE_FOLLOW:

            this->doFollowPathfinding( raycastResult );

            if (!raycastResult) {
                this->setAnimation( EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_WALK );
                this->state = EnemyState::ENEMY_STATE_STOP;
            } else {
                if ( raycastlength <= this->getRange() ) {
                    this->state = EnemyState::ENEMY_STATE_ATTACK;
                    this->setAnimation( EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_FIRE );
                }
            }
            break;
        case EnemyState::ENEMY_STATE_INJURIED:
            this->setAnimation( EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_INJURIED );
            if  (this->counterInjuried->isFinished() ) {
                this->state = EnemyState::ENEMY_STATE_STOP;
                this->setAnimation( EngineSetup::getInstance()->SpriteSoldierAnimations::SOLDIER_WALK );

            }
            break;
        case EnemyState::ENEMY_STATE_STOP:
            if ( raycastResult ) {
                this->state = EnemyState::ENEMY_STATE_FOLLOW;
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("enemyRage" + std::to_string(Tools::random(1, 5))), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
            }
            break;
        case EnemyState::ENEMY_STATE_DIE:
            break;
        default:
            assert(0);
    }
}

void NPCEnemyBody::doFollowPathfinding( bool raycastResult )
{
    if (!raycastResult) return;

    if (this->points.size() > 0) {
        btTransform t = this->getRigidBody()->getWorldTransform();
        btMotionState *mMotionState = this->getRigidBody()->getMotionState();

        btVector3 dest;
        Vector3D way = Vector3D(this->points[0], this->points[1]);

        Vertex3D p = way.getComponent().getNormalize().getScaled(this->getSpeed());
        Vertex3D pos = *this->getPosition() + p;

        if (!Tools::isValidVector(pos)) {
            return;
        }

        M3 newRot = M3::getFromVectors(way.getComponent().getNormalize(), EngineSetup::getInstance()->up);
        this->setRotation(newRot.getTranspose());
        pos.saveToBtVector3(&dest);

        t.setOrigin(dest);
        mMotionState->setWorldTransform(t);
        this->getRigidBody()->setWorldTransform(t);
    }
}

void NPCEnemyBody::syncPathFindingRotation()
{
    if (this->points.size() > 0) {
        Vector3D way = Vector3D(this->points[0], this->points[1]);

        M3 newRot = M3::getFromVectors(way.getComponent().getNormalize(), EngineSetup::getInstance()->up);
        this->setRotation(newRot.getTranspose());
    }
}

void NPCEnemyBody::shoot(Camera3D *cam, btDiscreteDynamicsWorld *dynamicsWorld, std::vector<Object3D*> &gameObjects)
{
    Projectile3DBody *projectile = new Projectile3DBody();
    projectile->setFromEnemy( true );
    projectile->setPosition( *this->getPosition() );
    projectile->setLabel("projectile");
    projectile->setEnabled(true);
    projectile->linkTexturesTo( this->projectileTemplate );
    projectile->setAnimation(0);
    Vector3D dir = Vector3D(*this->getPosition(), *cam->getPosition());
    projectile->makeProjectileRigidBodyToPlayer(1, gameObjects, dir.getComponent().getNormalize(), dynamicsWorld, 700);
    projectile->getBillboard()->setDimensions(0.5, 0.5);

    if (this->points.size() > 0) {
        Vector3D way = Vector3D(this->points[0], this->points[1]);

        M3 newRot = M3::getFromVectors(way.getComponent().getNormalize(), EngineSetup::getInstance()->up);
        projectile->setRotation(newRot.getTranspose());
    }

    Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("bulletWhisper"), EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);

}

void NPCEnemyBody::updateCounters()
{
    this->counterCadence->update();
    this->counterIA->update();
    this->counterInjuried->update();
}

void NPCEnemyBody::respawn()
{
    this->stamina = this->startStamina;
    this->dead    = false;

    btMotionState *mMotionState = this->getRigidBody()->getMotionState();

    btTransform trans;
    trans.setIdentity();

    Vertex3D pos = this->getRespawnPosition();
    this->setPosition( this->getRespawnPosition() );

    trans.setOrigin(btVector3(pos.x , pos.y, pos.z));

    mMotionState->setWorldTransform( trans );
    this->getRigidBody()->setWorldTransform( trans );
    this->setRotation( this->getRespawnRotation() );
}
