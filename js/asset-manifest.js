(function (global) {
  'use strict';

  global.FEED_BRIDE_ASSETS = {
    music: {
      stage1: '1.mp3',
      stage2: '2.mp3',
      stage3: '3.mp3',
      stage4: '4.mp3',
      stage5: '5.mp3'
    },
    images: 'images/',
    videos: 'videos/',
    intro: 'Intro.mp4'
    // TODO: assets/ 구조 완전 전환 후 아래로 변경
    // stage1: 'assets/music/1.mp3', ...
    // images: 'assets/images/', videos: 'assets/videos/'
  };

  // 핫픽스: stage2.html 884행이 event_gamble_stare.png를 호출하지만
  // 깃허브 실제 파일은 event_gamble_stare.jpg 라서 로드 실패함.
  // png 로드 실패 시 jpg로 1회 자동 대체. (stage2.html 수정 시 이 블록 삭제 가능)
  document.addEventListener('error', function (e) {
    var t = e.target;
    if (t && t.tagName === 'IMG' && t.src && t.src.indexOf('event_gamble_stare.png') !== -1 && !t.__jpgRetry) {
      t.__jpgRetry = true;
      t.src = t.src.replace('event_gamble_stare.png', 'event_gamble_stare.jpg');
    }
  }, true);
})(window);
