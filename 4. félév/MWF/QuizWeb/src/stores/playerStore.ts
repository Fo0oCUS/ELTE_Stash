import { defineStore } from "pinia";
import { ref } from "vue";
import type { JoinRequestDto } from "../Models/requestDtos";

const SELECTED_ANSWER_INDEX_KEY = "selectedAnswerIndex";
const PLAYER_NAME_KEY = "playerName";
const JOIN_CODE_KEY = "joinCode";
const QUESTION_INDEX_KEY = "questionIndex";

export const usePlayerStore = defineStore("playerStore", () => {
  const playerName = ref("");
  const joinCode = ref("");
  const selectedAnswerIndex = ref<number | null>(null);
  const questionIndex = ref<number>(-1);

  function setSelectedAnswerIndex(index: number | null) {
    selectedAnswerIndex.value = index;

    if (index === null) {
      localStorage.removeItem(SELECTED_ANSWER_INDEX_KEY);
      return;
    }

    localStorage.setItem(SELECTED_ANSWER_INDEX_KEY, String(index));
  }

  function getSelectedAnswerIndex(): number | null {
    return selectedAnswerIndex.value;
  }

  function clearSelectedAnswerIndex() {
    setSelectedAnswerIndex(null);
  }

  function setQuestionIndex(index: number) {
    questionIndex.value = index;
    localStorage.setItem(QUESTION_INDEX_KEY, String(index));
  }

  function getQuestionIndex(): number {
    return questionIndex.value;
  }

  function initialize(data: JoinRequestDto) {
    playerName.value = data.playerName;
    joinCode.value = data.joinCode;
    setQuestionIndex(-1);

    localStorage.setItem(PLAYER_NAME_KEY, data.playerName);
    localStorage.setItem(JOIN_CODE_KEY, data.joinCode);

    clearSelectedAnswerIndex();
  }

  function clearPlayerStore() {
    playerName.value = "";
    joinCode.value = "";
    selectedAnswerIndex.value = null;

    localStorage.removeItem(PLAYER_NAME_KEY);
    localStorage.removeItem(JOIN_CODE_KEY);
    localStorage.removeItem(SELECTED_ANSWER_INDEX_KEY);
  }

  function loadFromLocalStorage() {
    playerName.value = localStorage.getItem(PLAYER_NAME_KEY) ?? "";
    joinCode.value = localStorage.getItem(JOIN_CODE_KEY) ?? "";
    questionIndex.value = Number(localStorage.getItem(QUESTION_INDEX_KEY)) ?? -1;

    const storedIndex = localStorage.getItem(SELECTED_ANSWER_INDEX_KEY);
    selectedAnswerIndex.value =
      storedIndex !== null ? Number(storedIndex) : null;
  }
  
  return {
    playerName,
    joinCode,
    selectedAnswerIndex,
    setSelectedAnswerIndex,
    getSelectedAnswerIndex,
    clearSelectedAnswerIndex,
    clearPlayerStore,
    setQuestionIndex,
    getQuestionIndex,
    initialize,
    loadFromLocalStorage
  };
});
