package edu.duke.raft;

import java.util.Random;
import java.util.Timer;

public class CandidateMode extends RaftMode {

    private static final int TIMER_ID = 3;
    private static final int HEARTBEAT = 6;

    private int timeoutValue;
    private Timer myTimer = new Timer();
    private Timer myTimer2 = new Timer();
    private Random r = new Random();
    private boolean switched = false;

    public void go() {
        synchronized (mLock) {
            System.out.println("S" +
                    mID +
                    "." +
                    mConfig.getCurrentTerm() +
                    ": switched to candidate mode.");

            timeoutValue = mConfig.getTimeoutOverride() > 0
                    ? mConfig.getTimeoutOverride()
                    : r.nextInt(ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN) + ELECTION_TIMEOUT_MIN;

            myTimer = scheduleTimer(timeoutValue, TIMER_ID);
            RaftResponses.setTerm(mConfig.getCurrentTerm());
            sendVoteRequests();
        }
    }

    private void sendVoteRequests() {
        if (!RaftResponses.clearVotes(mConfig.getCurrentTerm())) {
            myTimer.cancel();
            myTimer2.cancel();
            switched = true;
            RaftServerImpl.setMode(new FollowerMode());
            return;
        }

        if (myTimer2 != null) myTimer2.cancel();
        myTimer2 = scheduleTimer(HEARTBEAT_INTERVAL, HEARTBEAT);

        for (int i = 1; i <= mConfig.getNumServers(); i++) {
            //System.out.println(String.format("Sending vote to %d from %d", i, mID));
            if (mID != i) remoteRequestVote(i, mConfig.getCurrentTerm(), mID, mLog.getLastIndex(), mLog.getLastTerm());
        }
    }

    // @param candidate’s term
    // @param candidate requesting vote
    // @param index of candidate’s last log entry
    // @param term of candidate’s last log entry
    // @return 0, if server votes for candidate; otherwise, server's
    // current term
    public int requestVote(int candidateTerm,
                           int candidateID,
                           int lastLogIndex,
                           int lastLogTerm) {
        synchronized (mLock) {
            int term = mConfig.getCurrentTerm();
            int result = term;

//            if (candidateTerm > term) {
//                result = candidateTerm;
//
//                if (mLog.getLastTerm() < lastLogTerm || mLog.getLastIndex() <= lastLogIndex) {
//                    mConfig.setCurrentTerm(candidateTerm, candidateID);
//                    result = 0;
//                } else {
//                    mConfig.setCurrentTerm(candidateTerm, 0);
//                }
//
//                myTimer.cancel();
//                myTimer2.cancel();
//                RaftServerImpl.setMode(new FollowerMode());
//            }

            if (term < candidateTerm || mLog.getLastTerm() < lastLogTerm || mLog.getLastIndex() < lastLogIndex) {
                myTimer.cancel();
                myTimer2.cancel();
                mConfig.setCurrentTerm(candidateTerm, candidateID);

                result = 0;
                switched = true;
                RaftServerImpl.setMode(new FollowerMode());
            }

            return result;
        }
    }


    // @param leader’s term
    // @param current leader
    // @param index of log entry before entries to append
    // @param term of log entry before entries to append
    // @param entries to append (in order of 0 to append.length-1)
    // @param index of highest committed entry
    // @return 0, if server appended entries; otherwise, server's
    // current term
    public int appendEntries(int leaderTerm,
                             int leaderID,
                             int prevLogIndex,
                             int prevLogTerm,
                             Entry[] entries,
                             int leaderCommit) {
        synchronized (mLock) {
            int term = mConfig.getCurrentTerm();
            int result = term;
            boolean changeMode = false;

            if (leaderTerm > mConfig.getCurrentTerm()) {
                result = leaderTerm;

                if (entries == null || entries.length == 0) {
                    if (prevLogIndex == mLog.getLastIndex() && prevLogTerm == mLog.getLastTerm()) {
                        result = 0;
                    }
                    changeMode = true;
                }

                if (mLog.insert(entries, prevLogIndex, prevLogTerm) != -1) {
                    result = 0;
                    changeMode = true;
                }

                if (changeMode) {
                    myTimer.cancel();
                    myTimer2.cancel();
                    switched = true;
                    mConfig.setCurrentTerm(leaderTerm, 0);
                    RaftServerImpl.setMode(new FollowerMode());
                }

            }

            return result;
        }
    }

    // @param id of the timer that timed out
    public void handleTimeout(int timerID) {
        synchronized (mLock) {
            if (switched) {
                return;
            }

            int[] votes;
            if (timerID == HEARTBEAT) {
                if ((votes = RaftResponses.getVotes(mConfig.getCurrentTerm())) != null) {
                    //System.out.println(String.format("Server %d is counting votes", mID));
                    //System.out.println(Arrays.toString(votes));

                    double voteCount = 1;
                    for (int i : votes) {
                        if (i == 0) {
                            voteCount++;
                        } else if (i > mConfig.getCurrentTerm()) {
                            myTimer.cancel();
                            myTimer2.cancel();
                            mConfig.setCurrentTerm(i, 0);
                            switched = true;
                            RaftServerImpl.setMode(new FollowerMode());
                            return;
                        }
                    }

                    if (voteCount / mConfig.getNumServers() >= .5) {
                        myTimer.cancel();
                        myTimer2.cancel();
                        switched = true;
                        RaftServerImpl.setMode(new LeaderMode());
                        return;
                    }
                } else {
                    myTimer.cancel();
                    myTimer2.cancel();
                    switched = true;
                    RaftServerImpl.setMode(new FollowerMode());
                    return;
                }
            } else if (timerID == TIMER_ID) {
                myTimer.cancel();
                myTimer2.cancel();
                mConfig.setCurrentTerm(mConfig.getCurrentTerm() + 1, mID);
                RaftResponses.setTerm(mConfig.getCurrentTerm());
                myTimer = scheduleTimer(timeoutValue, TIMER_ID);
            }
            sendVoteRequests();
        }
    }
}
