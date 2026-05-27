export type LoginRequestDto = {
    email: string,
    password: string
}

export type QuizRequestDto = {
    title: string,
    questions: QuestionRequestDto[]
}

export type QuestionRequestDto = {
    title: string,
    answers: string[],
    correctAnswerIndex: number
}

export type GetQuizByJoinCodeRequestDto = {
    playerName: string,
    joinCode: string
}

export type JoinRequestDto = {
    joinCode: string,
    playerName: string
}

export type RegisterRequestDto = {
    name: string,
    email: string,
    password: string
}

export type QuizMessageRequestDto = {
    playerName: string,
    joinCode: string,
    message: string
}

