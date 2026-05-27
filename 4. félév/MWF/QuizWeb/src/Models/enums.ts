export const QuizStatus = {
  waitingToBePublished: "WaitingToBePublished",
  lobby: "Lobby",
  inProgress: "InProgress",
  finished: "Finished",
} as const

export type QuizStatus =
  (typeof QuizStatus)[keyof typeof QuizStatus]