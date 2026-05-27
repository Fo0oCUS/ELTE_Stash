import type { QuizStatus } from "./enums"

export type LoginResponseDto = {
    userId: string,
    authToken: string,
    refreshToken: string
}

export type UserResponseDto = {
    id: string,
    name: string,
    email: string
}

export type QuizResponseDto = {
    id: number,
    title: string,
    questions: QuestionResponseDto[],
    messages: string[],
    currentQuestionIndex: number,
    isPublished: boolean,
    joinCode?: string | null,
    status: QuizStatus
}

export type QuizResponseForPlayerDto = {
    title: string,
    questions: QuestionResponseForPlayerDto[],
    messages: string[],
    currentQuestionIndex: number,
    isPublished: boolean,
    joinCode?: string | null,
    status: QuizStatus
}

export type QuestionResponseDto = {
    title: string,
    answers: string[]
    correctAnswerIndex: number,
    isOpen: boolean
}

export type QuestionResponseForPlayerDto = {
    title: string,
    answers: string[]
    isOpen: boolean
}