import { Enum } from 'cc';

export namespace CharacterTypes {
    export enum State {
        Idle,
        Move,
        Attack,
        Dead
    }
    Enum(State);
}
