import { _decorator, AnimationClip, CCFloat, Component, Node, Animation, AnimationState } from 'cc';
import { CharacterTypes } from './CharacterTypes';
import { Character } from './Character';
const { ccclass, property } = _decorator;

@ccclass('CharacterStateAnimation')
class CharacterStateAnimation {
    @property({ type: CharacterTypes.State })
    public state: CharacterTypes.State = CharacterTypes.State.Idle;

    @property(AnimationClip)
    public clip: AnimationClip = null;
}

@ccclass('CharacterAnimationController')
export class CharacterAnimationController extends Component {
    public static FINISHED_ANIMATION:string = 'FINISHED_ANIMATION';

    @property([CharacterStateAnimation])
    private animationList: CharacterStateAnimation[] = [];

    @property(CCFloat)
    private crossFadeDuration: number = 0.3;

    private _animation: Animation = null;
    private _animationClips: Map<CharacterTypes.State, AnimationClip> = new Map<CharacterTypes.State, AnimationClip>();
    private _emitEventNode: Node = null;

    protected onLoad(): void {
        this._animation = this.getComponent(Animation);
        this._animation.on(Animation.EventType.FINISHED, this.onAnimationFinished, this);

        this.initEmitEventNode();
        this.initAnimationClips();
    }

    protected onDestroy(): void {
        this._animation.off(Animation.EventType.FINISHED, this.onAnimationFinished, this);
        this._animationClips.clear();
    }

    private onAnimationFinished(type: Animation.EventType, state: AnimationState): void {
        this._emitEventNode.emit(CharacterAnimationController.FINISHED_ANIMATION, type, state);
    }

    private initEmitEventNode(): void {
        let character: Character = this.getComponent(Character);
        if (character == null) {
            character = this.node.parent.getComponent(Character);
        }
        this._emitEventNode = character.node;
    }

    private initAnimationClips(): void {
        for (let i = 0, animationCount = this.animationList.length; i < animationCount; ++i) {
            const animation = this.animationList[i];
            this._animationClips.set(animation.state, animation.clip);
        }
    }

    private crossFadeAnimation(state: CharacterTypes.State): void {
        const aniClip = this._animationClips.get(state);
        this._animation.crossFade(aniClip.name, this.crossFadeDuration);
    }

    private playAnmation(state: CharacterTypes.State): void {
        const aniClip = this._animationClips.get(state);
        this._animation.play(aniClip.name);
    }

    public isAnyState(state: CharacterTypes.State): boolean {
        return state == CharacterTypes.State.Attack;
    }

    public setState(state: CharacterTypes.State): void {
        switch (state) {
            case CharacterTypes.State.Idle:
            case CharacterTypes.State.Move:
                this.crossFadeAnimation(state);
                break;
            case CharacterTypes.State.Attack:
                this.playAnmation(state);
                break;
        }
    }

    public equalAnimationByStateAndClipName(state: CharacterTypes.State, clipName: string): boolean {
        const animationClip = this._animationClips.get(state);
        return animationClip.name == clipName;
    }
}
