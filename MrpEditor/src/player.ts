
import MidiPlayer from 'midi-player-js';
import Soundfont from 'soundfont-player';


// declare module 'midi.js' {
// declare namespace Midi {
//     interface MidiEvent {
//         deltaTime: number;
//         type: string;
//         subtype?: string;
//         channel?: number;
//         noteNumber?: number;
//         velocity?: number;
//         amount?: number;
//         controllerType?: number;
//         value?: number;
//         programNumber?: number;
//         text?: string;
//         number?: number;
//         microsecondsPerBeat?: number;
//         numerator?: number;
//         denominator?: number;
//         metronome?: number;
//         thirtyseconds?: number;
//         key?: number;
//         scale?: number;
//         data?: Uint8Array;
//         frameRate?: number;
//         hour?: number;
//         min?: number;
//         sec?: number;
//         frame?: number;
//         subframe?: number;
//     }

//     interface MidiHeader {
//         formatType: number;
//         trackCount: number;
//         ticksPerBeat: number;
//     }

//     interface MidiFile {
//         header: MidiHeader;
//         tracks: MidiEvent[][];
//     }

//     interface Generator {
//         alive: boolean;
//         released?: boolean;
//         generate(buf: number[], offset: number, count: number): void;
//         noteOff?(velocity?: number): void;
//     }

//     interface Program {
//         createNote(note: number, velocity: number): Generator;
//         attackAmplitude?: number;
//         sustainAmplitude?: number;
//         attackTime?: number;
//         decayTime?: number;
//         releaseTime?: number;
//     }

//     interface Replayer {
//         generate(samples: number): number[];
//         finished: boolean;
//     }

//     function MidiFile(data: ArrayBuffer): MidiFile;
//     function Replayer(midiFile: MidiFile): Replayer;
//     function setSampleRate(rate: number): void;
// }

// interface MidiPlayer {
//     isInited: boolean;
//     generator: Midi.Replayer | null;
//     midi: Midi.MidiFile | null;
//     isLoop: boolean;
//     node: ScriptProcessorNode | null;
//     context: AudioContext | null;

//     init(): void;
//     play(midiData: ArrayBuffer, isLoop: boolean): void;
//     stop(): void;
// }

// declare function MidiPlayer(): MidiPlayer;

// }

export class Player {
    private filename: String | undefined;
    private data: Uint8Array | undefined;
    private midiPlayer: MidiPlayer.Player | undefined;
    private audio: HTMLAudioElement|undefined;
    private audioContext: AudioContext | undefined;
    private instrument: any;

    private isPlaying: boolean = false;
    private musicControl: HTMLElement;
    private playBtn: HTMLElement;
    private stopBtn: HTMLElement;
    constructor() {
        // 获取DOM元素
        this.musicControl = document.getElementById('musicControl')!;
        this.playBtn = document.getElementById('playBtn')!;
        this.stopBtn = document.getElementById('stopBtn')!;
        
        // 绑定事件
        this.playBtn.addEventListener('click', () => this.play());
        this.stopBtn.addEventListener('click', () => this.stop());
    }

    public setData(data:Uint8Array, filename:String){
        this.data = data;
        this.filename = filename;
    }

    // 加载并播放MIDI文件
    // public playMidi(midiData: ArrayBuffer): void {
    //     this.isPlaying = true;
    //     this.updateButtonStates();
    //     this.show();
    // }

    // 切换播放/暂停
    private togglePlay(): void {
        this.isPlaying = !this.isPlaying;
        this.updateButtonStates();
    }

    // 停止播放
    // private stop(): void {
    //     this.player.stop();
    //     this.isPlaying = false;
    //     this.updateButtonStates();
    // }

    // 更新按钮状态
    private updateButtonStates(): void {
        const playIcon = this.playBtn.querySelector('i')!;
        if (this.isPlaying) {
            playIcon.classList.remove('fa-play');
            playIcon.classList.add('fa-pause');
        } else {
            playIcon.classList.remove('fa-pause');
            playIcon.classList.add('fa-play');
        }
    }

    // 显示控制按钮
    public show(): void {
        this.musicControl.style.display = 'block';
    }

    // 隐藏控制按钮
    public hide(): void {
        this.musicControl.style.display = 'none';
    }

    /**
* 检查数据是否为有效的 MIDI 文件
* @param data 可能是 ArrayBuffer 或 Uint8Array 的数据
* @returns 如果是有效的 MIDI 文件返回 true，否则返回 false
*/
    private isMIDIFile(data: ArrayBuffer | Uint8Array): boolean {
        // 如果是 ArrayBuffer，转换为 Uint8Array
        const uint8Data = data instanceof ArrayBuffer
            ? new Uint8Array(data)
            : data;

        if (!(uint8Data instanceof Uint8Array)) {
            return false;
        }

        if (uint8Data.length < 8) {
            return false;
        }

        // MIDI 文件必须以 "MThd" 字节开头 (ASCII: 77, 84, 104, 100)
        const midiHeader = new Uint8Array([77, 84, 104, 100]);

        // 比较前四个字节
        for (let i = 0; i < midiHeader.length; i++) {
            if (uint8Data[i] !== midiHeader[i]) {
                return false;
            }
        }

        return true;
    }

    /**
     * 检查数据是否为有效的 WAV 文件
     * @param data 可能是 ArrayBuffer 或 Uint8Array 的数据
     * @returns 如果是有效的 WAV 文件返回 true，否则返回 false
     */
    private isWAVFile(data: ArrayBuffer | Uint8Array): boolean {
        // 如果是 ArrayBuffer，转换为 Uint8Array
        const uint8Data = data instanceof ArrayBuffer
            ? new Uint8Array(data)
            : data;

        if (!(uint8Data instanceof Uint8Array)) {
            return false;
        }

        if (uint8Data.length < 12) {  // WAV 需要至少 12 字节检查
            return false;
        }

        // WAV 文件的前四个字节必须是 "RIFF" (ASCII: 82, 73, 70, 70)
        const riffHeader = new Uint8Array([82, 73, 70, 70]);

        // 比较前四个字节
        for (let i = 0; i < riffHeader.length; i++) {
            if (uint8Data[i] !== riffHeader[i]) {
                return false;
            }
        }

        // 检查 8-11 字节是否是 "WAVE" (ASCII: 87, 65, 86, 69)
        const waveHeader = new Uint8Array([87, 65, 86, 69]);
        for (let i = 8; i < 12; i++) {
            if (uint8Data[i] !== waveHeader[i - 8]) {
                return false;
            }
        }

        return true;
    }

    // 播放音乐
    public async play() {
        this.show();
        this.isPlaying = true;
        this.updateButtonStates();
        if(this.isMIDIFile(this.data!)){
             console.log("play midi..........");
             try {
                 // 初始化Web Audio Context
                 await this.setupAudioContext();
                 
                 // 创建MIDI播放器
                 this.midiPlayer = new MidiPlayer.Player();
                 
                 // 加载MIDI数据
                 this.midiPlayer.loadArrayBuffer(this.data!.buffer as ArrayBuffer);
                 
                 // 设置MIDI事件处理
                 this.midiPlayer.on('midiEvent', (event: any) => {
                     this.handleMidiEvent(event);
                 });
                 
                 // 播放
                 this.midiPlayer.play();
                 
                 console.log("MIDI player started successfully");
             } catch (error) {
                 console.error("MIDI playback failed:", error);
                 this.isPlaying = false;
                 this.updateButtonStates();
                 this.hide();
             }
        } else {
            console.log("play wav.........");
            let isWav = this.isWAVFile(this.data!);
            this.audio = new Audio(URL.createObjectURL(new Blob([new Uint8Array(this.data!)], { type: isWav ? 'audio/wav' : 'audio/mp3' })));
            this.audio.play();
        }
    }

    // 停止播放
    public stop() {
        this.hide();
        this.isPlaying = false;
        this.updateButtonStates();
        if(this.midiPlayer){
            this.midiPlayer.stop();
            this.midiPlayer = undefined;
        }
        if(this.audio){
            this.audio.pause();
            
            this.audio.currentTime = 0; // 重置音频到开始位置
            this.audio = undefined;
        }
    }

    // 设置Web Audio API上下文用于MIDI播放
    private async setupAudioContext(): Promise<void> {
        try {
            this.audioContext = new AudioContext();
            
            // 加载钢琴音源
            this.instrument = await Soundfont.instrument(this.audioContext, 'acoustic_grand_piano');
            
            console.log("Audio context and instrument loaded successfully");
        } catch (error) {
            console.error("Failed to setup audio context:", error);
            throw error;
        }
    }

    // 处理MIDI事件
    private handleMidiEvent(event: any): void {
        if (!this.instrument) return;

        if (event.name === 'Note on') {
            this.instrument.play(event.noteName);
        } else if (event.name === 'Note off') {
            this.instrument.stop(event.noteName);
        }
    }

}