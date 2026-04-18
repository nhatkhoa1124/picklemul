import React, { useEffect, useRef, useCallback, useState } from 'react';
import { View, Text, NativeModules, StyleSheet, TouchableOpacity, ScrollView } from 'react-native';
import { Canvas, Path, Skia } from '@shopify/react-native-skia';
import { ROMS } from './src/roms';

const { Chip8Native } = NativeModules;

const CHIP8_W = 64;
const CHIP8_H = 32;
const TARGET_CPU_HZ = 1200;
const TARGET_TIMER_HZ = 60;
const MAX_FRAME_DELTA_MS = 100;
const MAX_CYCLES_PER_FRAME = 80;

const CHIP8_KEYPAD_ROWS = [
  [
    { label: '1', hexKey: 0x1 },
    { label: '2', hexKey: 0x2 },
    { label: '3', hexKey: 0x3 },
    { label: '4', hexKey: 0xC },
  ],
  [
    { label: 'Q', hexKey: 0x4 },
    { label: 'W', hexKey: 0x5 },
    { label: 'E', hexKey: 0x6 },
    { label: 'R', hexKey: 0xD },
  ],
  [
    { label: 'A', hexKey: 0x7 },
    { label: 'S', hexKey: 0x8 },
    { label: 'D', hexKey: 0x9 },
    { label: 'F', hexKey: 0xE },
  ],
  [
    { label: 'Z', hexKey: 0xA },
    { label: 'X', hexKey: 0x0 },
    { label: 'C', hexKey: 0xB },
    { label: 'V', hexKey: 0xF },
  ],
] as const;

type ControllerButtonProps = {
  title: string;
  hexKey: number;
  style?: object;
};

const ControllerButton = ({ title, hexKey, style }: ControllerButtonProps) => {
  const handlePressIn  = useCallback(() => Chip8Native.setKey(hexKey, true),  [hexKey]);
  const handlePressOut = useCallback(() => Chip8Native.setKey(hexKey, false), [hexKey]);

  return (
    <TouchableOpacity
      activeOpacity={0.6}
      onPressIn={handlePressIn}
      onPressOut={handlePressOut}
      style={[styles.button, style]}
    >
      <Text style={styles.buttonText}>{title}</Text>
    </TouchableOpacity>
  );
};

const CANVAS_W = 360;
const CANVAS_H = 180;
const PIXEL_W  = CANVAS_W / CHIP8_W; 
const PIXEL_H  = CANVAS_H / CHIP8_H; 

function App(): React.JSX.Element {
  const rafRef = useRef<number>(0);
  const runningRef = useRef(false);
  const lastFrameMsRef = useRef<number | null>(null);
  const cycleCarryRef = useRef(0);
  const timerCarryRef = useRef(0);
  const [isRunning, setIsRunning] = useState(false);
  const [screenPath, setScreenPath] = useState(Skia.Path.Make());
  const [activeRomName, setActiveRomName] = useState<string>('IBM Logo');

  const renderFrame = useCallback((frameTimeMs: number) => {
    if (!runningRef.current) {
      return;
    }

    try {
      if (lastFrameMsRef.current === null) {
        lastFrameMsRef.current = frameTimeMs;
      }

      const rawDeltaMs = frameTimeMs - lastFrameMsRef.current;
      const clampedDeltaMs = Math.max(0, Math.min(rawDeltaMs, MAX_FRAME_DELTA_MS));
      lastFrameMsRef.current = frameTimeMs;

      const deltaSeconds = clampedDeltaMs / 1000;

      cycleCarryRef.current += deltaSeconds * TARGET_CPU_HZ;
      cycleCarryRef.current = Math.min(cycleCarryRef.current, MAX_CYCLES_PER_FRAME);
      const cyclesToRun = Math.floor(cycleCarryRef.current);
      cycleCarryRef.current -= cyclesToRun;

      for (let i = 0; i < cyclesToRun; i++) {
        Chip8Native.execCycle();
      }

      timerCarryRef.current += deltaSeconds * TARGET_TIMER_HZ;
      const timerTicks = Math.floor(timerCarryRef.current);
      timerCarryRef.current -= timerTicks;

      for (let i = 0; i < timerTicks; i++) {
        Chip8Native.updateTimers();
      }

      const buffer: number[] = Chip8Native.getDisplayBuffer();
      const path = Skia.Path.Make();

      for (let i = 0; i < buffer.length; i++) {
        if (buffer[i] === 1 || buffer[i] === 255) {
          const x = (i % CHIP8_W) * PIXEL_W;
          const y = Math.floor(i / CHIP8_W) * PIXEL_H;
          path.addRect(Skia.XYWHRect(x, y, PIXEL_W, PIXEL_H));
        }
      }

      setScreenPath(path);
    } catch (error) {
      console.warn('Frame execution failed:', error);
      runningRef.current = false;
      setIsRunning(false);
      return;
    }

    if (runningRef.current) {
      rafRef.current = requestAnimationFrame(renderFrame);
    }
  }, []);

  const bootGame = useCallback((romName: string) => {
    runningRef.current = false;
    cancelAnimationFrame(rafRef.current);
    lastFrameMsRef.current = null;
    cycleCarryRef.current = 0;
    timerCarryRef.current = 0;

    const romData = ROMS[romName];
    if (!Array.isArray(romData) || romData.length === 0) {
      console.warn(`ROM ${romName} is empty! Please add the byte array.`);
      setIsRunning(false);
      return;
    }

    const sanitizedRomData = romData
      .filter((value) => Number.isFinite(value))
      .map((value) => {
        const normalized = Math.trunc(Number(value));
        return ((normalized % 256) + 256) % 256;
      });

    if (sanitizedRomData.length === 0) {
      console.warn(`ROM ${romName} has no valid bytes.`);
      setIsRunning(false);
      return;
    }

    try {
      Chip8Native.init();
      Chip8Native.loadRom(sanitizedRomData);

      lastFrameMsRef.current = null;
      cycleCarryRef.current = 0;
      timerCarryRef.current = 0;
      setScreenPath(Skia.Path.Make());
      setActiveRomName(romName);
      runningRef.current = true;
      setIsRunning(true);
      rafRef.current = requestAnimationFrame(renderFrame);
    } catch (error) {
      console.warn(`Failed to load ROM ${romName}:`, error);
      runningRef.current = false;
      setIsRunning(false);
    }
  }, [renderFrame]);

  const stopEngine = () => {
    runningRef.current = false;
    cancelAnimationFrame(rafRef.current);
    lastFrameMsRef.current = null;
    cycleCarryRef.current = 0;
    timerCarryRef.current = 0;
    setIsRunning(false);
    setScreenPath(Skia.Path.Make());
  };

  useEffect(() => {
    return () => {
      runningRef.current = false;
      cancelAnimationFrame(rafRef.current);
      lastFrameMsRef.current = null;
      cycleCarryRef.current = 0;
      timerCarryRef.current = 0;
    };
  }, []);

  return (
    <View style={styles.container}>

      <Text style={styles.title}>PicklEmul</Text>

      <View style={styles.carouselContainer}>
        <ScrollView horizontal showsHorizontalScrollIndicator={false} contentContainerStyle={styles.carousel}>
          {Object.keys(ROMS).map((romName) => (
            <TouchableOpacity 
              key={romName} 
              style={[styles.cartridgeButton, activeRomName === romName && isRunning && styles.cartridgeActive]}
              onPress={() => bootGame(romName)}
            >
              <Text style={[styles.cartridgeText, activeRomName === romName && isRunning && styles.cartridgeTextActive]}>
                {romName}
              </Text>
            </TouchableOpacity>
          ))}
        </ScrollView>
      </View>

      <View style={styles.screenWrapper}>
        <Canvas style={styles.screen}>
          <Path path={screenPath} color="#33FF33" />
        </Canvas>
      </View>

      <Text style={styles.statusText}>{isRunning ? `Running: ${activeRomName}` : 'Stopped'}</Text>

      <View style={styles.systemControls}>
        <TouchableOpacity style={styles.pillButton} onPress={stopEngine}>
          <Text style={styles.buttonText}>STOP</Text>
        </TouchableOpacity>
      </View>

      <View style={styles.keypadContainer}>
        {CHIP8_KEYPAD_ROWS.map((row, rowIndex) => (
          <View key={`row-${rowIndex}`} style={styles.keypadRow}>
            {row.map((keyDef) => (
              <ControllerButton
                key={keyDef.label}
                title={keyDef.label}
                hexKey={keyDef.hexKey}
                style={styles.keyButton}
              />
            ))}
          </View>
        ))}

        <Text style={styles.keypadHint}>CHIP-8 layout mapped to 1-4 / Q-R / A-F / Z-V</Text>
      </View>

    </View>
  );
}

const styles = StyleSheet.create({
  container: { 
    flex: 1, 
    backgroundColor: '#1e1e1e', 
    alignItems: 'center', 
    paddingTop: 40, 
    paddingBottom: 20 
  },
  title: { 
    fontSize: 20, 
    fontWeight: 'bold', 
    color: '#4DA8DA', 
    marginBottom: 15, 
    letterSpacing: 2 
  },
  carouselContainer: { 
    height: 50, 
    width: '100%', 
    marginBottom: 15 
  },
  carousel: { 
    paddingHorizontal: 20, 
    alignItems: 'center' 
  },
  cartridgeButton: { 
    paddingHorizontal: 20, 
    paddingVertical: 8, 
    backgroundColor: '#333', 
    borderRadius: 20, 
    marginRight: 10, 
    borderWidth: 1, 
    borderColor: '#444' 
  },
  cartridgeActive: { 
    backgroundColor: '#4DA8DA', 
    borderColor: '#4DA8DA' 
  },
  cartridgeText: { 
    color: '#aaa', 
    fontWeight: 'bold' 
  },
  cartridgeTextActive: { 
    color: '#000' 
  },
  screenWrapper: { 
    width: CANVAS_W, 
    height: CANVAS_H, 
    backgroundColor: '#0f0f0f', 
    marginBottom: 16 
  },
  screen: { 
    width: '100%', 
    height: '100%' 
  },
  statusText: {
    color: '#9aa4b2',
    fontWeight: '600',
    marginBottom: 10,
  },
  button: { 
    justifyContent: 'center', 
    alignItems: 'center' 
  },
  keypadContainer: {
    width: '100%',
    alignItems: 'center',
    marginTop: 8,
  },
  keypadRow: {
    flexDirection: 'row',
    marginBottom: 8,
  },
  keyButton: {
    width: 64,
    height: 52,
    backgroundColor: '#2f3542',
    borderRadius: 10,
    marginHorizontal: 6,
    borderWidth: 1,
    borderColor: '#57606f',
  },
  keypadHint: {
    marginTop: 4,
    color: '#8f9bb3',
    fontSize: 12,
  },
  systemControls: { 
    flexDirection: 'row', 
    width: '100%', 
    justifyContent: 'center', 
    marginBottom: 12 
  },
  pillButton: { 
    width: 86, 
    height: 32, 
    backgroundColor: '#555', 
    borderRadius: 12.5, 
    justifyContent: 'center', 
    alignItems: 'center' 
  },
  buttonText: { 
    color: '#fff', 
    fontWeight: 'bold', 
    fontSize: 16 
  },
});

export default App;