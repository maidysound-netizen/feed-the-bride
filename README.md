# FEED the WKB / Feed the Bride

정적 HTML 기반의 스토리·방치형 인터랙티브 게임입니다.

## 현재 실행 방법

프로젝트 루트에서 정적 서버를 실행합니다.

```bash
python3 -m http.server 8000
```

브라우저에서 [http://localhost:8000/index.html](http://localhost:8000/index.html)을 엽니다.

## 현재 스테이지

- `index.html`: Stage 1
- `stage2.html`: Stage 2
- `stage3.html`: Stage 3
- `stage4.html`: Stage 4
- `stage5.html`: Stage 5
- `stage6.html`: Stage 6

현재 ZIP에는 Stage 7 HTML과 Stage 6·7 음악 파일이 포함되어 있지 않습니다. 해당 파일은 임의로 생성하지 않습니다.

## 분리된 asset 구조

새 작업용 asset은 다음 위치에 보존됩니다.

```text
assets/
├── images/
├── music/
└── videos/
```

`js/asset-manifest.js`는 스테이지별 음악과 공통 asset 경로를 관리하는 단일 manifest입니다. 기존 루트 asset은 호환성을 위해 당분간 유지합니다.

Stage 4 음악은 반드시 `assets/music/4.mp3`를 사용합니다.

## 진행 상태

현재 각 스테이지 HTML 내부에 게임 로직이 포함되어 있습니다. 다음 단계에서 공통 저장 상태, 오디오, 이벤트 재생기를 `js/`로 이동하고, 음식·대사·이벤트 데이터는 `data/`로 분리할 예정입니다.
