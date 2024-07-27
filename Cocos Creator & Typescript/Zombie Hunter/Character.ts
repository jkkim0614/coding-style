import { _decorator, Component, Sprite, math, Collider2D, CircleCollider2D, RigidBody2D, CCFloat, Contact2DType, IPhysics2DContact, Vec2, Vec3 } from 'cc';
import { CharacterAnimationController } from './CharacterAnimationController';
import { CharacterTypes } from './CharacterTypes';
const { ccclass, property, requireComponent } = _decorator;

@ccclass('Character')
@requireComponent([RigidBody2D, CircleCollider2D])
export class Character extends Component {
    public static BEGIN_PHYSICS_CONTACT:string = 'BEGIN_PHYSICS_CONTACT';
    public static END_PHYSICS_CONTACT:string = 'END_PHYSICS_CONTACT';
    public static DIE_CHARACTER:string = 'DIE_CHARACTER';

    @property({ group: { name: 'Base' }, type: CharacterAnimationController })
    public animationController: CharacterAnimationController = null;

    @property({ group: { name: 'Movement' }, type: CCFloat })
    public moveSpeed: number = 1;

    @property({ group: { name: 'Combat' }, type: CCFloat })
    private healthPoint: number = 0;

    @property({ group: { name: 'Combat' }, type: CCFloat })
    public attackDamage: number = 0;

    @property({ group: { name: 'Combat' }, type: CCFloat })
    public detectTargetDistance: number = 100;

    private _rigidBody: RigidBody2D = null;
    private _collider: CircleCollider2D = null;
    private _sprite: Sprite = null;
    private _stopVelocity: Vec2 = new Vec2(0, 0);
    private _state: CharacterTypes.State = CharacterTypes.State.Idle;
    private _hp: number = 0;
    
    get rigidBody() { return this._rigidBody; }
    get collider() { return this._collider; }
    get sprite() { return this._sprite; }

    get state() { return this._state; }
    set state(state: CharacterTypes.State) {
        if (this._state == state) { return; }
        
        this.animationController?.setState(state);
        this._state = state;
    }

    protected onLoad(): void {
        this._rigidBody = this.getComponent(RigidBody2D);
        this._collider = this.getComponent(CircleCollider2D);
        this._sprite = this.getComponentInChildren(Sprite);
        this._collider.on(Contact2DType.BEGIN_CONTACT, this.onBeginContact, this);
        this._collider.on(Contact2DType.END_CONTACT, this.onEndContact, this);

        this._hp = this.healthPoint;
    }

    protected onDestroy(): void {
        this._collider.off(Contact2DType.BEGIN_CONTACT, this.onBeginContact, this);
        this._collider.off(Contact2DType.END_CONTACT, this.onEndContact, this);
    }

    private onBeginContact(selfCollider: Collider2D, otherCollider: Collider2D, contact: IPhysics2DContact | null): void {
        this.node.emit(Character.BEGIN_PHYSICS_CONTACT, selfCollider, otherCollider, contact);
    }

    private onEndContact(selfCollider: Collider2D, otherCollider: Collider2D, contact: IPhysics2DContact | null): void {
        this.node.emit(Character.END_PHYSICS_CONTACT, selfCollider, otherCollider, contact);
    }

    protected onStateChanged(state: CharacterTypes.State): void {
        this.animationController?.setState(state);
    }

    public isDead(): boolean {
        return this.state == CharacterTypes.State.Dead;
    }

    public revive(): void {
        this._hp = this.healthPoint;
        this.state = CharacterTypes.State.Idle;
    }

    public move(direction: Vec2, moveRatio: number): void {
        const velocity = direction.multiplyScalar(this.moveSpeed * moveRatio);
        this._rigidBody.linearVelocity = velocity;
        this.state = CharacterTypes.State.Move;
    }

    public moveVector3(direction: Vec3, moveRatio: number): void {
        const directionVec2: Vec2 = new Vec2(direction.x, direction.y);
        this.move(directionVec2, moveRatio);
    }

    public movePosition(direction: Vec2, moveRatio: number): void {
        let direction3 = new Vec3(direction.x, direction.y);
        const moveDirection = direction3.multiplyScalar(moveRatio * this.moveSpeed);
        let currentPos = this.node.position;

        this._rigidBody.linearVelocity = direction;
        this.node.setPosition(currentPos.add(moveDirection));
    }

    public stop(): void {
        this._rigidBody.linearVelocity = this._stopVelocity;
        this.state = CharacterTypes.State.Idle;
    }

    public attack():void {
        this.state = CharacterTypes.State.Attack;
    }

    public damaged(otherDamage: number): void {
        this._hp -= otherDamage;
        if (this._hp <= 0) {
            this.die();
        }
    }

    public die(): void {
        this.node.emit(Character.DIE_CHARACTER);
        this.state = CharacterTypes.State.Dead;
    }
}
