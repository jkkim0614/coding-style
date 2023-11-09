import { _decorator, Component, math, Node, Animation, AnimationState } from 'cc';
import { ObjectPoolManager, PoolObject, PoolObjectType } from '../manager/ObjectPoolManager';
const { ccclass } = _decorator;

@ccclass('BulletHit')
export class BulletHit extends Component implements PoolObject {
    private _animation: Animation = null;
    public objectType: PoolObjectType = PoolObjectType.BulletHit;
    public objectNode: Node = null;

    protected onLoad(): void {
        this._animation = this.getComponent(Animation);
        this._animation.on(Animation.EventType.FINISHED, this.onAnimationFinished, this);

        this.objectNode = this.node;
    }

    protected onDestroy(): void {
        this._animation.off(Animation.EventType.FINISHED, this.onAnimationFinished, this);
    }

    private onAnimationFinished(type: Animation.EventType, state: AnimationState): void {
        this.returnPool();
    }

    public resetPosition(position: math.Vec3): void {
        this.node.setPosition(position);
    }

    public setActive(active: boolean): void {
    }

    public returnPool(): void {
        ObjectPoolManager.instance.push(this.objectType, this);
    }

    public playAnimation(): void {
        this._animation.play();
    }
}
